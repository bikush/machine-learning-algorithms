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
#include <map>
#include <stdexcept>
#include <random>
#include <assert.h>

class Neural_network {
public:
	Neural_network(const std::vector<int> & init);
	int num_layers;
	std::map<int, int> neurons_per_layer;
	std::map<int, std::vector<Neuron>> neurons_at_layer;
	void operator() (const std::vector<std::vector<double>> &, const std::vector<std::vector<double>> &, int);
	void set_weight(double, int layer, int neuron_in, int neuron_out);
	void set_bias(double, int layer);

private:
	double generate_init_weight();
	void build_network();
	void calculate();
};
#endif /* NEURALNETWORK_H_ */
