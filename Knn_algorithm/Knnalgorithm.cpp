/*
 * Knnalgorithm.cpp
 *
 *  Created on: May 23, 2016
 *      Author: marija
 */

#include "Knnalgorithm.h"
using namespace  std;

multimap<double, int> Knn_algorithm::calculate_distances(const vector<double> & query_point) {
	int num_elems = inputs[0].size();
	vector<double> temp(inputs.size());
	multimap<double, int> distances;
	for (int i=0; i < num_elems; i++) {
		for (size_t j=0; j<inputs.size(); j++) {
			temp[j] = inputs[j][i];
		}
		distances.insert(make_pair(distance_func(temp, query_point), i));
	}
	return distances;
}

vector<double> Knn_algorithm::get_average(const multimap<double, int> & knn) {
	int counter = k;
	int n = 0;
	vector<double> res(outputs.size(), 0.0);
	for (auto m = knn.begin(); m != knn.end(); m++) {
		double distance = (*m).first;
		auto it = knn.equal_range(distance);
		for (auto el = it.first; el != it.second; el++) {
			n++;
			int idx = (*el).second;
			for (size_t i=0; i<outputs.size(); i++) {
				res[i] += outputs[i][idx];
			}
		}
		counter--;
		if (counter == 0) break;
	}
	for (size_t i=0; i<res.size(); i++) {
		res[i] = res[i]/n;
		cout << ((n == k) ? "k and n are the same ": "n is: ") << n << endl;
	}
	return res;
}

bool Knn_algorithm::check_database(const vector<double> & query_point, vector<double> & res){
	for (size_t i=0; i < inputs.size(); i++) {
		bool found = true;
		for (size_t j=0; j < inputs[0].size(); j++) {
			if (inputs[i][j] != query_point[i]) {
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

	/*get k neighbours*/
	multimap<double, int> knn = calculate_distances(query_point);
	res = get_average(knn);
	return res;
}
