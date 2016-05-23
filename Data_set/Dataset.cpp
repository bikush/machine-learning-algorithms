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

using namespace std;

void Data::print(){
	for (auto a : attributes) {
		cout << a << ": " << values_map[a] << endl;
	}
}

const size_t Attribute_set::get_number_of_inputs() const
{
	size_t number = 0;
	for (auto item : attr_usage) {
		if (item == Attribute::Attribute_usage::input) {
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
	std::cout << "Created data set " << l.c_str() << std::endl;
}

void Data_set::load_simple_db(const std::string & path)
{
	load_simple_db(path, label_name);
}

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
}

void Data_set::distribute_split(Data_set & first, Data_set & second, double percentage){
	percentage = std::min(1.0, std::max(0.0, percentage));
	first.attr = attr;
	first.label_name = label_name;
	second.attr = attr;
	second.label_name = label_name;
	int total_size = get_size();
	int first_size = int(percentage * total_size);
	for (int i = 0; i < first_size; i++) {
		first.data_set.push_back(data_set[i]);
	}
	for (int i = first_size; i < total_size; i++) {
		second.data_set.push_back(data_set[i]);
	}	
}

void Data_set::distribute_random(Data_set & first, Data_set & second, double percentage)
{
	// TODO: randomly select a percentage of elements into first and (1-percentage) into second
}

void Data_set::distribute_fold(Data_set & first, Data_set & second, int fold_count, int take_fold)
{
	// TODO: split date into fold_count parts, place take_fold-th fold into second, place rest into first
}

void Data_set::normalized_data(std::vector<std::vector<double>>& inputs, std::vector<std::vector<double>>& outputs) const
{
	// TODO: this is a complex problem that needs more thought
	int size = get_size();
	inputs.reserve(size);
	outputs.reserve(size);

	// TODO: move to Attribute_set class
	// TODO: add support for multiple outputs
	auto attr_inputs = attr.get_filtered_attributes();
	auto found_class_itr = find(attr_inputs.begin(), attr_inputs.end(), label_name);
	if (found_class_itr != attr_inputs.end()) {
		attr_inputs.erase(found_class_itr);
	}

	vector<double> transformed_data( attr_inputs.size() );
	vector<double> transformed_output(1);

	// TODO: move to Attribute_set
	// TODO: figure out what to do with numerical values
	map<string, map<string, double>> transform;
	auto attr_names = attr.get_all_attributes();
	for (auto attribute : attr_names) {
		auto values = attr.get_attr_values(attribute);
		map<string, double> attribute_transform;
		double transform_value = 0.0;
		double increment = 1.0 / (values.size()-1);
		for (string value : values) {
			attribute_transform[value] = transform_value;
			transform_value += increment;
		}
		transform[attribute] = attribute_transform;
	}

	
	for (auto data : data_set)
	{
		// TODO: data transform into a function
		transformed_data.clear();
		for (auto attribute : attr_inputs) {
			transformed_data.push_back(transform[attribute][data.get_value(attribute)]);
		}
		// TODO: an array of outputs/labels is needed
		transformed_output.clear();
		transformed_output.push_back(transform[label_name][data.get_value(label_name)]);
		
		inputs.push_back(transformed_data);
		outputs.push_back(transformed_output);
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

vector<string> Attribute_set::get_filtered_attributes() const {
	vector<string> res;
	for (size_t i=0; i<attr_names.size(); i++) {
		if (attr_used[i])
			res.push_back(attr_names[i]);
	}
	return res;
}

const set<string> & Attribute_set::get_attr_values(const string & attr) const{
	return attr_map.at(attr);
}
