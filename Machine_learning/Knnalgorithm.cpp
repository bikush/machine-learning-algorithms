/*
 * Knnalgorithm.cpp
 *
 *  Created on: May 23, 2016
 *      Author: marija
 */

#include "Knnalgorithm.h"
#include "Dataset.h"
#include <stdexcept>
#include <assert.h>

using namespace  std;

Knn_algorithm::Knn_algorithm(const std::vector<std::vector<double>> & in,
							 const std::vector<std::vector<double>> & out,
							 int nn, distance_function d, bool v)
			   :inputs{in}, outputs{out}, k{nn}, distance_func{d}, vote{v}{
	int in_s = in.size();
	int out_s = out.size();
	assert(in_s == out_s);
	assert(in_s > 0);
	assert(in_s > nn);
}

//const Data_set & data

Knn_algorithm::Knn_algorithm(const Data_set & data, int nn, distance_function d, bool v)
	:k{nn}, distance_func{d}, vote{v}
{
	Algorithm::learn(data);
}

void Knn_algorithm::classify(const Data & d, std::vector<double> & out) {

};
void Knn_algorithm::setup(const Algorithm_parameters& parameters) {

};
void Knn_algorithm::learn(const Data_set & data_set, const Attribute_normalizer& normalizer) {
	//return Knn_algorithm({}, {}, 0, distance_func, vote);
	this->normalizer = normalizer;
	this->normalizer.normalized_data(data_set, inputs, outputs);
	int in_s = inputs.size();
	int out_s = outputs.size();
	assert(in_s == out_s);
	assert(in_s > 0);
	assert(in_s > k);
};

void Knn_algorithm::test(const Data_set & test) {
	vector<vector<double>> in;
	vector<vector<double>> out;
	int miss_labeled = 0;
	int total = 0;
	auto attr_outputs = test.attr.get_attributes_of_kind(Attribute::Attribute_usage::output);
	normalizer.normalized_data(test,in, out);
	int in_s = in.size();
	int out_s = out.size();
	assert(in_s == out_s);
	assert(in_s > 0);
	for (size_t i=0; i<in.size(); i++) {
		vector<double> res = (*this)(in[i]);
		vector<double> target = out[i];
		auto res_labels = normalizer.undo_normalize(attr_outputs, res);
		auto target_labels = normalizer.undo_normalize(attr_outputs, target);
		for (size_t j=0; j < out[0].size(); j++) {
			if (res_labels[j].second != target_labels[j].second) {
				miss_labeled++;
			}
			total++;
		}
	}
	cout << "Miss labeled: " << miss_labeled << " total: " <<  total << endl;
	cout << "Error: " << double(miss_labeled)/in.size()*100.0 << "%."<< endl;
}
struct sort_distances {
	bool operator() (pair<double, int> left, pair<double, int> right) const {
		return left.first < right.first;
	}
};
vector<pair<double, int>> Knn_algorithm::calculate_distances(const vector<double> & query_point) {
	int num_elems = inputs.size();
	vector<pair<double, int>> distances;
	for (int i=0; i < num_elems; i++) {
		distances.push_back(make_pair(distance_func(inputs[i], query_point), i));
	}
	sort(distances.begin(), distances.end(), sort_distances());
	return distances;
}

vector<double> Knn_algorithm::get_average(const vector<pair<double, int>> & knn) {
	int counter = k+1;
	vector<int> n(outputs[0].size(), 0);
	vector<double> res(outputs[0].size(), 0.0);
    double d = -1;
	for (auto m = knn.begin(); m != knn.end(); m++) {
		double distance = (*m).first;
		if (abs(distance - d) > tolerance) {
			d = distance;
			counter--;
		}
		if (counter == 0) break;
		int idx = (*m).second;
		for (size_t i=0; i<res.size(); i++) {
			res[i] += outputs[idx][i];
			n[i]++;
		}
	}
	for (size_t i=0; i<res.size(); i++) {
		res[i] = res[i]/n[i];
		//cout << ((n[i] == k) ? "k and n are the same ": "n is: ") << n[i] << ", k is: " << k << endl;
	}
	return res;
}

vector<double> Knn_algorithm::find_most_common(const vector<pair<double, int>> & knn) {
	int counter = k+1;
	vector<vector<double>> temp;
	vector<double> res(outputs[0].size());
    double d = -1;
	for (auto m = knn.begin(); m != knn.end(); m++) {
		double distance = (*m).first;
		if (abs(distance - d) > tolerance) {
			d = distance;
			counter--;
		}
		if (counter == 0) break;
		int idx = (*m).second;
		temp.push_back(outputs[idx]);
	}
	map<double, int> values;
	for (size_t i=0; i < res.size(); i++) {
		values[temp[i][0]] = 1;
		double result = temp[i][0];
		int max = 0;
		for (size_t j=1; j<temp.size(); j++) {
			double new_val = temp[j][i];
			bool found = false;
			for (auto val: values) {
				if (abs(val.first-new_val) <= tolerance) {
					found = true;
					val.second++;
					if (max < val.second) {
						max = val.second;
						result = val.first;
					}
					break;
				}
			}
			if (!found) {
				values[new_val] = 1;
			}
		}
		res[i] = result;

	}
	return res;
}

bool Knn_algorithm::check_database(const vector<double> & query_point, vector<double> & res){
	for (size_t i=0; i < inputs.size(); i++) {
		bool found = true;
		for (size_t j=0; j < inputs[0].size(); j++) {
			if (abs(inputs[i][j] - query_point[j]) > tolerance) {
				found = false;
				break;
			}
		}
		if (found == true) {
		    res = outputs[i];
			return true;
		}
	}
	return false;
}

vector<double> Knn_algorithm::operator()(const vector<double> & query_point){
	/*check if point is already in database.*/
	vector<double> res;
	if (check_database(query_point, res)) return res;

	/*get k neighbors*/
	vector<pair<double, int>> knn = calculate_distances(query_point);
	if (vote)
		res = find_most_common(knn);
	else
		res = get_average(knn);
	return res;
}



/*******************
 * E X A M P L E S *
 *******************/


double distance_func(const vector<double> & x1, const vector<double> & x2) {
	return  sqrt(pow(x2[0] - x1[0], 2) + pow((x2[1] - x1[1]), 2));
}

double db_distance_func(const vector<double> & x1, const vector<double> & x2) {
	assert(x1.size() == x2.size());
	double count = 0.0;
	for (size_t i = 0; i < x1.size(); i++) {
		if (abs(x1[i] - x2[i]) <= Knn_algorithm::tolerance) count += 1.0;
	}
	return count > 0 ? 1. / count : 10e10;
}

void db_test(const string & file_name, const string & class_name, int k) {
	Data_set ds;
	ds.load_simple_db(file_name, class_name);
	Data_set train, test;
	ds.distribute_split(train, test, 0.8);
	Knn_algorithm knn{ train, k, &db_distance_func, true };
	knn.test(test);
}

void knn_zoo_example(int k = 2) {
	//Data_set::_test_normalize_columns();
	cout << "ZOO example" << endl;
	string file_name = "../data/zoo.txt";
	string class_name = "type";
	db_test(file_name, class_name, k);
	cout << endl;
}

void knn_tennis_example(int k = 2) {
	//Data_set::_test_normalize_columns();
	cout << "Tennis example" << endl;
	string file_name = "../data/tennis.txt";
	string class_name = "Play";
	db_test(file_name, class_name, k);
	cout << endl;
}

void knn_simple_example(int k = 2) {
	int num_samples = 100;
	vector<vector<double>> inputs(num_samples*num_samples, vector<double>(2));
	vector<vector<double>> outputs(num_samples*num_samples, vector<double>(1));
	/*example: y = x1^2 + x2*/
	for (int i = 0; i<num_samples; i++) {
		for (int j = 0; j < num_samples; j++) {
			inputs[i*num_samples + j][0] = i*0.2;
			inputs[i*num_samples + j][1] = j*0.2;
			outputs[i*num_samples + j][0] = i*i*0.2*0.2 + j*0.2;
		}
	}
	Knn_algorithm knn{ inputs, outputs, 1, &distance_func };
	double error = 0.;
	double max_error = 0.;
	double max_relative_error = 0.;
	int total = 0;
	int test_samples = 10;
	for (int i = 0; i<test_samples; i++) {
		for (int j = 0; j<test_samples; j++) {
			vector<double>  res = knn(vector<double>{i*0.1, j*0.1});
			total++;
			double real_res = i*i*0.1*0.1 + j*0.1;
			double err = abs(res[0] - real_res);
			error += err;
			max_error = max(max_error, err);
			if (abs(real_res) > 0.)
				max_relative_error = max(max_relative_error, abs(max_error / real_res));
		}
	}
	cout << "Simple example: y = x1^2 + x2." << endl;
	cout << "Average error: " << error / total << endl;
	cout << "Max error: " << max_error << endl;
	cout << "Max relative error: " << max_relative_error << endl << endl;
}

void Knn_algorithm::run_examples() {
	knn_simple_example(1);
	knn_zoo_example(3);
	knn_tennis_example();
}
