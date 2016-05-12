/*
 * Neuralnetwork.cpp
 *
 *  Created on: May 12, 2016
 *      Author: marija
 */

#include "Neuralnetwork.h"
using namespace std;

Neural_network::Neural_network(const vector<int> & init) {
	if (init.size() <= 2) throw runtime_error("Invalid initialization of neural network");
	num_layers = init.size();
	for (int i=0; i < num_layers; i++) {
		neurons_per_layer[i] = init[i];
	}
	/*add bias*/
	for (int i=0; i < num_layers-1; i++) {
		neurons_per_layer[i]++;
	}
	for (int i=0; i < num_layers; i++) {
		neurons_at_layer[i] = vector<Neuron>(neurons_per_layer[i]);
	}
	build_network();
}

double Neural_network::generate_init_weight() {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dist(0.3,0.7);
	return dist(gen);
}

void Neural_network::build_network() {
	for (int i=1; i < num_layers; i++) {
		for (int j=0; j<neurons_per_layer[i-1]; j++) {
			for (int k=0; k<neurons_per_layer[i]; k++) {
				Edge_key key{i-1, j, k};
				double w = generate_init_weight();
				neurons_at_layer[i-1][j].add_out_edge(w, key);
				neurons_at_layer[i][k].add_in_edge(w, key);
			}
		}
	}
}

void Neural_network::set_weight(double w, int layer, int neuron_in, int neuron_out){
	Edge_key k{layer, neuron_in, neuron_out};
	neurons_at_layer[layer][neuron_in].out_edges_weights(k) = w;
	neurons_at_layer[layer+1][neuron_out].in_edges_weights(k) = w;
}

void Neural_network::calculate() {
	for (int i=1; i < num_layers; i++) {
		for (int j=0; j<neurons_per_layer[i]; j++) {
			auto keys = neurons_at_layer[i][j].in_edges_keys();
			double res = 0.0;
			for (const auto & key: keys) {
				double out = neurons_at_layer[key.layer][key.in].get_out();
				double w =  neurons_at_layer[key.layer][key.in].out_edges_weights(key);
				res += out*w;
			}
			neurons_at_layer[i][j].set_out(res);
		}
	}
}

void Neural_network::operator() (const vector<vector<double>> & inputs, const vector<vector<double>> & outputs, int epoch_count){
	size_t data_size = inputs[0].size();
	for (size_t i=0; i< inputs.size(); i++) {
		assert(data_size == inputs[i].size());
	}
	for (size_t i=0; i< outputs.size(); i++) {
		assert(data_size == outputs[i].size());
	}
	assert(inputs.size() == neurons_at_layer[0].size()-1);
	assert(outputs.size() == neurons_at_layer[num_layers-1].size());

	vector <vector<double>> target(outputs.size()); // target outputs

	while(epoch_count--) {
		for (size_t current=0; current < data_size; current++) {
			/*init inputs*/
			for (size_t i=0; i < inputs.size(); i++) {
				neurons_at_layer[0][i].set_out(inputs[i][current]);
			}
			/*init target*/
			for (size_t i=0; i < outputs.size(); i++) {
				target[i].push_back(outputs[i][current]);
			}
			/*calculate outputs*/
			calculate();

		}

	}
}
