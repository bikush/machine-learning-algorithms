/*
 * Neuralnetwork.h
 *
 *  Created on: May 12, 2016
 *      Author: marija
 */

#ifndef NEURALNETWORK_H_
#define NEURALNETWORK_H_
#include "Neuron.h"
#include <vector>
#include <stdexcept>
#include <random>
#include <assert.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include "Algorithm.h"
#include "Dataset.h"

class Data_set;
class Attribute_normalizer;

class Neural_network : public Algorithm {
public:
	Neural_network():eta{0.01}, ok{0.01}, num_layers{0}, epoch_count{1000} {std::cout << "constructor" << std::endl;}
	Neural_network(Neural_network && nn);
	Neural_network& operator=(Neural_network && nn);

	Neural_network(const Neural_network & nn);
	Neural_network& operator=(const Neural_network & nn);


	/*const std::vector<int> & init, double learning_rate=0.01, double acceptable_error=0.01*/
	virtual void setup(const Algorithm_parameters& param);
	virtual void learn(const Data_set & data_set);
	virtual void learn(const Data_set & data_set, const Attribute_normalizer& normalizer);
	using Algorithm::learn;
	void classify(const Data & d, std::vector<double> & out);
	void operator() (const std::vector<std::vector<double>> &, const std::vector<std::vector<double>> &);
	double get_learning_rate(){return eta;}
	void set_learning_rate(double lr) {eta = lr;}
	static void run_examples();
	static double test(Neural_network & nn, const std::vector<double> & inputs, const std::vector<double> & targets);
	static double test(Neural_network & nn, const Data_set & test_set);
	std::vector<double> calculate(const std::vector<double> & inputs);
private:
	Algorithm_parameters parameters;
	double eta;
	double ok;
	int num_layers;
    std::vector<int> neurons_per_layer;
	std::vector<std::vector<Neuron>> neurons_at_layer;
	int epoch_count;
	std::vector<int> config;
	void calculate_outputs();
	bool back_propagation(const std::vector<double> & targets);
	double generate_init_weight();
	void build_network();
	bool is_bias(int layer, int idx){return (config[layer] == idx);}
	void set_weight(double w, const Edge_key & k);
	Attribute_normalizer data_normalizer;
};
#endif /* NEURALNETWORK_H_ */
