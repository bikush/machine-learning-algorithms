/*
 * Neuron.cpp
 *
 *  Created on: May 12, 2016
 *      Author: marija
 */

#include "Neuron.h"

using namespace std;

vector<Edge_key> Neuron::in_edges_keys(){
	vector<Edge_key> v;
	for (auto elem: edges_in) {
		v.push_back(elem.first);
	}
	return v;
}

vector<Edge_key> Neuron::out_edges_keys(){
	vector<Edge_key> v;
	for (auto elem: edges_out) {
		v.push_back(elem.first);
	}
	return v;
}
