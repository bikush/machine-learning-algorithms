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

class Neural_network {
public:
	Neural_network(const std::vector<int> & init, double learning_rate=0.01, double acceptable_error=0.0001);
	void operator() (const std::vector<std::vector<double>> &, const std::vector<std::vector<double>> &, int);
	std::vector<double> calculate(const std::vector<double> & inputs);
	double get_learning_rate(){return eta;}
	void set_learning_rate(double lr) {eta = lr;}
private:
	double eta;
	double ok;
	int num_layers;
	std::vector<int> neurons_per_layer;
	std::vector<std::vector<Neuron>> neurons_at_layer;
	bool back_propagation(const std::vector<double> & targets);
	double generate_init_weight();
	void build_network();
	void calculate_outputs();
	void set_weight(double, int layer, int neuron_in, int neuron_out);
};
#endif /* NEURALNETWORK_H_ */
