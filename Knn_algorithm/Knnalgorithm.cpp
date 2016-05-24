/*
 * Knnalgorithm.cpp
 *
 *  Created on: May 23, 2016
 *      Author: marija
 */

#include "Knnalgorithm.h"
using namespace  std;

struct sort_distances {
	bool operator() (pair<double, int> left, pair<double, int> right) const {
		return left.first < right.first;
	}
};
vector<pair<double, int>> Knn_algorithm::calculate_distances(const vector<double> & query_point) {
	int num_elems = inputs[0].size();
	vector<double> temp(inputs.size());
	vector<pair<double, int>> distances;
	for (int i=0; i < num_elems; i++) {
		for (size_t j=0; j<inputs.size(); j++) {
			temp[j] = inputs[j][i];
		}
		distances.push_back(make_pair(distance_func(temp, query_point), i));
	}
	sort(distances.begin(), distances.end(), sort_distances());
	return distances;
}

vector<double> Knn_algorithm::get_average(const vector<pair<double, int>> & knn) {
	int counter = k+1;
	int n = 0;
	vector<double> res(outputs.size(), 0.0);
    double d = -1;
	for (auto m = knn.begin(); m != knn.end(); m++) {
		double distance = (*m).first;
		if (abs(distance - d) > tolerance) {
			d = distance;
			counter--;
		}
		if (counter == 0) break;
		int idx = (*m).second;
		for (size_t i=0; i<outputs.size(); i++) {
			res[i] += outputs[i][idx];
		}
		n++;
	}
	for (size_t i=0; i<res.size(); i++) {
		res[i] = res[i]/n;
		cout << ((n == k) ? "k and n are the same ": "n is: ") << n << ", k is: " << k << endl;
	}
	return res;
}

bool Knn_algorithm::check_database(const vector<double> & query_point, vector<double> & res){
	for (size_t i=0; i < inputs.size(); i++) {
		bool found = true;
		for (size_t j=0; j < inputs[0].size(); j++) {
			if (abs(inputs[i][j] - query_point[i]) > tolerance) {
				found = false;
				break;
			}
		}
		if (found == true) {
		    for (size_t n=0; n<outputs.size(); n++)
		    	res[n] = outputs[n][i];
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
	res = get_average(knn);
	return res;
}
