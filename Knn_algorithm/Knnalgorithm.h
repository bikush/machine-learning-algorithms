/*
 * Knnalgorithm.h
 *
 *  Created on: May 23, 2016
 *      Author: marija
 */

#ifndef KNNALGORITHM_H_
#define KNNALGORITHM_H_

#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

class Knn_algorithm {
public:
	Knn_algorithm(const std::vector<std::vector<double>> & in,
				  const std::vector<std::vector<double>> & out,
				  int nn, double (*d)(const std::vector<double> &, const std::vector<double> &))
		:inputs{in}, outputs{out}, k{nn}, distance_func{d}{}
	std::vector<double> operator()(const std::vector<double> & query_point);
private:
	const std::vector<std::vector<double>> inputs;
	const std::vector<std::vector<double>> outputs;
	int k;
	double (*distance_func)(const std::vector<double> &, const std::vector<double> &);
	std::multimap<double, int> calculate_distances(const std::vector<double> & query_point);
	std::vector<double> get_average(const std::multimap<double, int> & knn);
	bool check_database(const std::vector<double> & query_point, std::vector<double> &);
};

#endif /* KNNALGORITHM_H_ */
