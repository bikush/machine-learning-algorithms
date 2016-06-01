/*
 * Dataset.cpp
 *
 *  Created on: May 5, 2016
 *      Author: marija
 */

#include "Dataset.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <random>
#include <chrono>

using namespace std;

void Data::print() const{
	for (auto a : attributes) {
		cout << a << ": " << values_map.at(a) << endl;
	}
}

const int Attribute_set::count_attr_by_usage( Attribute::Attribute_usage usage ) const
{
	size_t number = 0;
	for (auto item : attr_usage) {
		if (item == usage) {
			number++;
		}
	}
	return number;
}

istream & operator>>(istream & is, Data & d) {
	for (const auto & attr : d.attributes) {
		string val;
		if (is >> val) {
			d.set_value(attr, val);
		}
		else {
			throw runtime_error("Invalid data format");
		}
	}
	return is;
}

// TODO: move to a utility header

// trim from start
static inline string &ltrim(string &s) {
	s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
	return s;
}

// trim from end
static inline string &rtrim(string &s) {
	s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
	return s;
}

// trim from both ends
static inline string &trim(string &s) {
	return ltrim(rtrim(s));
}

// END TODO

Data_set::Data_set(const std::string & l) :label_name{ l } {
	//std::cout << "Created data set " << l.c_str() << std::endl;
}

void Data_set::load_simple_db(const std::string & path)
{
	load_simple_db(path, label_name);
	set_init_weights();
}

//TODO: fix this to calculate real distribution over data!
void Data_set::set_init_weights(){
	size_t len = data_set.size();
	weights.reserve(len);
	for (size_t i=0; i<len; i++) {
		weights[i] = make_pair(i, 1./len);
	}
}

// TODO: use some other format for db
void Data_set::load_simple_db(const std::string & path, const std::string & class_name){
	ifstream ifs{ path };
	if (!ifs) {
		throw runtime_error("Input file not found.");
	}
	label_name = class_name;

	string line;
	bool db_flag = true;
	bool metadata = false;
	string db_name;
	while (getline(ifs, line)) {
		line = trim(line);
		if (line != "" && line[0] != ';') {
			if (db_flag) {
				db_flag = false;
				metadata = true;
				db_name = line;
				// the database name is the first line
			}
			else {
				istringstream is{ line };
				if (metadata) {
					metadata = false;
					string attr_val;
					string attr_type;
					while (is >> attr_val && is >> attr_type) {
						// TODO: fix this special case
						Attribute::Attribute_usage attr_usage = Attribute::Attribute_usage::input;
						if (attr_type == "name") {
							attr_usage = Attribute::Attribute_usage::none;
						}
						else if (attr_val == class_name) {
							attr_usage = Attribute::Attribute_usage::output;
						}
						Attribute new_attr{ attr_val, attr_type, attr_usage };
						attr.append_attr(new_attr);
					}
				}
				else {
					Data d{ attr.get_all_attributes() };
					is >> d;
					append_data(d);
				}
			}

		}

	}
	attr.generate_normalizer();
}

void Data_set::shuffle_data()
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(data_set.begin(), data_set.end(), default_random_engine{ seed });
}

void Data_set::fill_subset(Data_set & subset, const std::vector<int>& subset_indice) const
{
	subset.attr = attr;
	subset.label_name = label_name;
	subset.data_set.clear();
	subset.data_set.reserve(subset_indice.size());
	for (auto index : subset_indice) {
		subset.data_set.push_back(data_set[index]);
	}
	subset.set_init_weights();
}

void Data_set::distribute_split(Data_set & first, Data_set & second, double percentage, bool random) const{
	percentage = std::min(1.0, std::max(0.0, percentage));
	int total_size = get_size();
	int first_size = int(percentage * total_size);

	std::vector<int> indice(total_size);
	for (int i = 0; i < total_size; i++) {
		indice[i] = i;
	}
	if (random) {
		//auto clock = std::chrono::high_resolution_clock{};
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		shuffle(indice.begin(), indice.end(), default_random_engine{ seed });
	}

	fill_subset(first, std::vector<int>(indice.begin(), indice.begin() + first_size));
	fill_subset(second, std::vector<int>(indice.begin()+first_size, indice.end()));
}

//
// @param fold_count: number of folds the data will be split into.
// @param take_fold: fold index that will be inserted into the second data set. Starts from 0.
//
void Data_set::distribute_fold(Data_set & first, Data_set & second, int fold_count, int take_fold) const
{
	// Examples of folds
	// * * * * * * * * 8
	// * * *|* * *|* * (3)
	//-----------------------
	// * * * * * * * * * 9
	// * * *|* *|* *|* * (4)
	//---------------------------------
	// * * * * * * * * * * * * * * 14
	// * * *|* * *|* * *|* * *|* * (5)

	
	int total_size = get_size();
	fold_count = max(2, min(fold_count, total_size));
	take_fold = max(0, min(take_fold, fold_count - 1));
	int min_fold_size = total_size/fold_count;	// size of the smallest fold
	int overflow = total_size - min_fold_size * fold_count;	// number of leftover elements
	
	// if the fold to take is within the overflow, the size increases
	int main_fold_size = min_fold_size + ((take_fold < overflow) ? 1 : 0);
	// fold start index
	int fold_start = take_fold * min_fold_size + max(0, min(take_fold, overflow - 1));
	// fold end index
	int fold_end = fold_start + main_fold_size - 1;
	
	std::vector<int> indice(total_size - main_fold_size);
	std::vector<int> indice_fold(main_fold_size);
	int non_fold_idx = 0;
	int fold_idx = 0;
	for (int i = 0; i < total_size; i++) {
		if (i >= fold_start && i <= fold_end) {
			indice_fold[fold_idx] = i;
			fold_idx++;
		}
		else {
			indice[non_fold_idx] = i;
			non_fold_idx++;
		}
	}
	
	fill_subset(first, indice);
	fill_subset(second, indice_fold);
}

struct sort_weights {
	bool operator() (pair<int, double> left, pair<int, double> right) const {
		return left.second < right.second;
	}
};

void Data_set::distribute_boosting(Data_set & new_ds, vector<pair<int, double>> & new_weights) {
	/*
	 * 1. sort new_weights
	 * from weights, create new vector distribution <int index, int num_of_repetition>
	 * fill new_ds from old by vector distribution
	 * */
	assert(new_weights.size() == data_set.size());

	sort(new_weights.begin(), new_weights.end(), sort_weights{});
	int len = get_size();
	int new_len = 0;
	vector<pair<int,int>> distribution{};
	for (const auto & w: weights) {
		int batch = round(w.second*len);
		new_len += batch;
		if (new_len > len) {
			if ((new_len - batch) > 0) {
				new_len = len;
				batch = len - new_len + batch;
				distribution.push_back(make_pair(w.first, batch));
			}
			break;
		}
		distribution.push_back(make_pair(w.first, batch));
	}

	new_ds.attr = attr;
	new_ds.label_name = label_name;
	new_ds.data_set.clear();
	new_ds.data_set.reserve(new_len);

	for (auto d : distribution) {
		for (int i=0; i<d.second; i++)
		new_ds.data_set.push_back(data_set[d.first]);
	}
	new_ds.set_init_weights(); /*
	in this case wight init values will be incorrect
	since uniform distrib is used, but since this weights
	are not used in boosting alg, we do not care.
	Better solution would be if init distrib would be correct!
	*/

}

void Attribute_normalizer::add_attribute(const std::string & attr_name, const std::set<std::string> & values)
{
	map<string, double> attribute_transform;
	double transform_value = 0.0;
	double increment = 1.0 / (values.size() - 1);
	for (string value : values) {
		attribute_transform[value] = transform_value;
		transform_value += increment;
	}
	transform[attr_name] = attribute_transform;
	span[attr_name] = increment;
}

void Attribute_normalizer::normalize(const Data & data, const std::vector<std::string>& attributes, std::vector<double>& output)
{
	output.clear();
	for (auto attribute : attributes) {
		output.push_back(transform[attribute][data.get_value(attribute)]);
	}
}

vector<pair<double, string>> Attribute_normalizer::undo_normalize(const vector<string>& attributes, const vector<double>& values)
{
	vector<pair<double, string>> output;

	for (unsigned int i = 0; i < attributes.size(); i++) {
		string attr_name = attributes[i];
		double value = values[i];

		auto attr_trans = transform[attr_name];
		double best_fit = attr_trans.begin()->second;
		string best_name = attr_trans.begin()->first;
		for (auto trans_pair : attr_trans) {
			if (abs(trans_pair.second - value) < abs(best_fit - value)) {
				best_fit = trans_pair.second;
				best_name = trans_pair.first;
			}
		}

		double accuracy = abs(best_fit - value) / span[attr_name];
		output.push_back({ accuracy, best_name });
	}

	return output;
}

void Attribute_set::generate_normalizer() {
	normalizer.reset();
	// TODO: figure out what to do with numerical values
	
	for (auto attribute : attr_names) {
		normalizer.add_attribute(attribute, get_attr_values(attribute));
	}
}


// TODO: a lot of c/p code, fix this
void Data_set::normalized_data(std::vector<std::vector<double>>& inputs, std::vector<std::vector<double>>& outputs) const
{
	// TODO: this is a complex problem that needs more thought, or?
	int size = get_size();
	inputs.reserve(size);
	outputs.reserve(size);

	auto attr_inputs = attr.get_attributes_of_kind(Attribute::Attribute_usage::input);
	auto attr_outputs = attr.get_attributes_of_kind(Attribute::Attribute_usage::output);
	
	vector<double> transformed_data(attr_inputs.size());
	vector<double> transformed_output(attr_outputs.size());

	auto normalizer = attr.get_normalizer();
	for (auto data : data_set)
	{
		normalizer.normalize(data, attr_inputs, transformed_data);
		inputs.push_back(transformed_data);

		normalizer.normalize(data, attr_outputs, transformed_output);
		outputs.push_back(transformed_output);
	}
}

// TODO: a lot of c/p code, fix this
void Data_set::normalized_data_columns(std::vector<std::vector<double>>& inputs, std::vector<std::vector<double>>& outputs) const
{
	vector<vector<double>> row_inputs, row_outputs;
	normalized_data(row_inputs, row_outputs);

	int attr_input_size = attr.count_attr_by_usage(Attribute::Attribute_usage::input);
	int attr_output_size = attr.count_attr_by_usage(Attribute::Attribute_usage::output);

	int size = get_size();
	for (int input_idx = 0; input_idx < attr_input_size; input_idx++) {
		inputs.push_back(vector<double>(size));
		for (int data_idx = 0; data_idx < size; data_idx++) {
			inputs[input_idx][data_idx] = row_inputs[data_idx][input_idx];
		}
	}
	for (int output_idx = 0; output_idx < attr_output_size; output_idx++) {
		outputs.push_back(vector<double>(size));
		for (int data_idx = 0; data_idx < size; data_idx++) {
			outputs[output_idx][data_idx] = row_outputs[data_idx][output_idx];
		}
	}
}

vector<int> Data_set::split_by_attr_val(const vector<int> & subset, const string & att, const string & val) const {
	vector<int> idxs;
	for (size_t i=0; i<subset.size(); i++) {
		int idx = subset[i];
		// TODO: valid idx?
		if (data_set[idx].get_value(att) == val) {
			idxs.push_back(idx);
		}
	}
	return idxs;
}

void Data_set::define_attr_values(const Data & d){
	for (const auto & att: attr.get_all_attributes()) {
		auto it = attr.get_attr_map().find(att);
		string val = d.get_value(att);
		if (it == attr.get_attr_map().end()) {
			attr.get_attr_map()[att] = set<string>{{val}};
		} else {
			attr.get_attr_map()[att].insert(val);
		}
	}
}

void Data_set::clear()
{
	label_name = "";
	data_set.clear();
	attr = Attribute_set{};
}

void Data_set::_test_normalize()
{
	string file_name = "../data/tennis.txt";
	string class_name = "Play";  // type
	string name = "Day";
	Data_set ds;
	ds.load_simple_db(file_name, class_name);

	auto all_attr = ds.attr.get_all_attributes();
	for (auto attr : all_attr) {
		cout << attr << ": ";
		auto att_vals = ds.attr.get_attr_values(attr);
		for (auto val : att_vals) {
			cout << val << ", ";
		}
		cout << "increment: " << 1.0 / (att_vals.size() - 1) << endl;
	}
	cout << endl;

	vector<vector<double>> inputs, outputs;
	ds.normalized_data(inputs, outputs);
	auto input_attrs = ds.attr.get_attributes_of_kind(Attribute::Attribute_usage::input);
	for (unsigned int i = 0; i < inputs.size(); i++) {
		Data data = ds.get_elem(i);
		data.print();
		auto& elem = inputs[i];
		for (auto val : elem) {
			cout << val << ", ";
		}
		cout << " class: " << outputs[i][0] << endl << "--------------------------------------" << endl;

		elem[0] = abs(elem[0] - 0.2);
		auto norm = ds.attr.get_normalizer();
		auto undo = norm.undo_normalize(input_attrs, elem);
		int undo_idx = 0;
		for (auto u_item : undo) {
			cout << input_attrs[undo_idx] << ": " << u_item.second << ", error: " << u_item.first << endl;
			undo_idx++;
		}

		cout << endl << endl;
	}
}

void Data_set::_test_normalize_columns()
{
	string file_name = "../data/tennis.txt";
	string class_name = "Play";  // type
	string name = "Day";
	Data_set ds;
	ds.load_simple_db(file_name, class_name);

	auto all_attr = ds.attr.get_all_attributes();
	for (auto attr : all_attr) {
		cout << attr << ": ";
		auto att_vals = ds.attr.get_attr_values(attr);
		for (auto val : att_vals) {
			cout << val << ", ";
		}
		cout << "increment: " << 1.0 / (att_vals.size() - 1) << endl;
	}
	cout << endl;

	vector<vector<double>> inputs, outputs;
	ds.normalized_data_columns(inputs, outputs);

	/*cout << "COUNT " << ds.get_size() << endl;
	cout << "COLUMNS " << inputs.size() << endl;
	cout << "COLUMN SIZE " << inputs[0].size() << endl;
	for (int i = 0; i < inputs.size(); i++) {
		for (int j = 0; j < inputs[i].size(); j++) {
			cout << inputs[i][j] << " ";
		}
		cout << endl;
	}*/

	auto input_attrs = ds.attr.get_attributes_of_kind(Attribute::Attribute_usage::input);
	for (int i = 0; i < ds.get_size(); i++) {
		Data data = ds.get_elem(i);
		data.print();
		vector<double> row;
		for (unsigned int attr_idx = 0; attr_idx < inputs.size(); attr_idx++) {
			cout << inputs[attr_idx][i] << ", ";
			row.push_back(inputs[attr_idx][i]);
		}
		cout << " class: " << outputs[0][i] << endl << "--------------------------------------" << endl;

		inputs[0][i] = abs(inputs[0][i] - 0.2);
		auto norm = ds.attr.get_normalizer();
		auto undo = norm.undo_normalize(input_attrs, row);
		int undo_idx = 0;
		for (auto u_item : undo) {
			cout << input_attrs[undo_idx] << ": " << u_item.second << ", error: " << u_item.first << endl;
			undo_idx++;
		}
		cout << endl << endl;
	}
}

void Data_set::_test_distribute_fold()
{
	string file_name = "../data/tennis.txt";
	string class_name = "Play";
	Data_set ds;
	ds.load_simple_db(file_name, class_name);

	Data_set first;
	Data_set second;

	cout << "Total size: " << ds.get_size() << endl;
	for (int i = 2; i <= ds.get_size() / 2; i++) {
		cout << "Fold count " << i << endl;
		for (int j = 0; j < i; j++) {
			ds.distribute_fold(first, second, i, j);
			cout << "    take " << j << ", taken " << first.get_size() << " and " << second.get_size() << endl;
		}
	}
}

vector<string> Attribute_set::get_attributes_of_kind(Attribute::Attribute_usage usage) const
{
	vector<string> res;
	for (size_t i=0; i<attr_names.size(); i++) {
		if (attr_usage[i] == usage) {
			res.push_back(attr_names[i]);
		}
	}
	return res;
}

const set<string> & Attribute_set::get_attr_values(const string & attr) const{
	return attr_map.at(attr);
}
