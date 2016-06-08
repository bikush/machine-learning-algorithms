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
#include <memory>
#include "Algorithm.h"
class Data_set;

class Boosting: public Algorithm {
public:
	Boosting(const std::vector<Algorithm_parameters> parameters):alg_params{parameters}{}
	void setup(const Algorithm_parameters& parameters) {boosting_params = parameters;}
	void learn(const Data_set & data_set);
	void classify(const Data & d, double & out);
	void classify_all(const Data_set & test_set);
	static void run_examples();
private:
    void calculate_new_distribution(Algorithm* alg, Data_set & set);
	std::vector<std::pair<std::shared_ptr<Algorithm>, double>> algorithms;
	std::vector<Algorithm_parameters> alg_params;
	Algorithm_parameters boosting_params;
};

#endif /* BOOSTING_H_ */
