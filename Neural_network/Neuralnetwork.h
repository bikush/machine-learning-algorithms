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

class Data_set;

class Neural_network {
public:
	Neural_network(const std::vector<int> & init, double learning_rate=0.01, double acceptable_error=0.01);
	void operator() (const std::vector<std::vector<double>> &, const std::vector<std::vector<double>> &, int);
	void operator() (const Data_set &, int);
	double get_learning_rate(){return eta;}
	void set_learning_rate(double lr) {eta = lr;}
	static double test(Neural_network & nn, const std::vector<double> & inputs, const std::vector<double> & targets);
	static double test(Neural_network & nn, const Data_set & test_set);
	std::vector<double> calculate(const std::vector<double> & inputs);
private:
	double eta;
	double ok;
	int num_layers;
	std::vector<int> config;
	std::vector<int> neurons_per_layer;
	std::vector<std::vector<Neuron>> neurons_at_layer;
	bool back_propagation(const std::vector<double> & targets);
	double generate_init_weight();
	void build_network();
	void calculate_outputs();
	bool is_bias(int layer, int idx){return (config[layer] == idx);}
	void set_weight(double w, const Edge_key & k);
};
#endif /* NEURALNETWORK_H_ */
