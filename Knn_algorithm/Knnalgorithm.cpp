/*
 * Knnalgorithm.cpp
 *
 *  Created on: May 23, 2016
 *      Author: marija
 */

#include "Knnalgorithm.h"
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
	:k{nn}, distance_func{d}, vote{v}{
	data.normalized_data(inputs, outputs);
	int in_s = inputs.size();
	int out_s = outputs.size();
	assert(in_s == out_s);
	assert(in_s > 0);
	assert(in_s > nn);
}

void Knn_algorithm::test(const Data_set & test) {
	vector<vector<double>> in;
	vector<vector<double>> out;
	int miss_labeled = 0;
	int total = 0;
	auto normalizer = test.attr.get_normalizer();
	auto attr_outputs = test.attr.get_attributes_of_kind(Attribute::Attribute_usage::output);
	test.normalized_data(in, out);
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
