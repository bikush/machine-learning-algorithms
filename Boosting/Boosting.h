/*
 * Boosting.h
 *
 *  Created on: Jun 1, 2016
 *      Author: marija
 */

#ifndef BOOSTING_H_
#define BOOSTING_H_

#include <vector>
#include <math.h>
class Data_set;
#include "Neuralnetwork.h"

class Boosting {
public:
	Boosting(int n=4): num_iterations{n}{}
    void operator()(const Data_set &);
    void classify(const Data_set &);

	static void run_examples();
private:
    double calculate_new_distribution(Neural_network & nn, Data_set & set);
	int num_iterations;
	std::vector<std::pair<Neural_network, double>> algorithms;
};

#endif /* BOOSTING_H_ */
