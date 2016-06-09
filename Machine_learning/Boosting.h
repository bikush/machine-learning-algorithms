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
#include "Dataset.h"

class Boosting: public Algorithm {
public:
	Boosting(const std::vector<Algorithm_parameters> parameters):alg_params{parameters}, num_algorithms{0}{}
	void setup(const Algorithm_parameters& parameters) {boosting_params = parameters;}
	void learn(const Data_set & data_set);
	void learn(const Data_set &ds, const  Attribute_normalizer & a_nom);
	void classify(const Data & d, std::vector<double> & out);
	void classify_all(const Data_set & test_set);
	static void run_examples();
private:
	void common(const Data_set &ds);
    bool calculate_new_distribution(Algorithm* alg, Data_set & set);
    double calculate_true_error(Algorithm & alg, const Data_set & test_set, std::vector<int> & res_v);
	std::vector<std::pair<std::shared_ptr<Algorithm>, double>> algorithms;
	std::vector<Algorithm_parameters> alg_params;
	int num_algorithms = 0;
	Algorithm_parameters boosting_params;
	Attribute_normalizer data_normalizer;
};

#endif /* BOOSTING_H_ */
