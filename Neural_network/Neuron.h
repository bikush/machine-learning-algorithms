/*
 * Neuron.h
 *
 *  Created on: May 12, 2016
 *      Author: marija
 */

#ifndef NEURON_H_
#define NEURON_H_
#include <map>
#include <vector>
#include <sstream>

struct Edge_key {
	Edge_key(int l=0, int i=0, int o=0): layer{l}, in{i}, out{o}{}
	int layer;
	int in;
	int out;
	bool operator<(const Edge_key& key) const{
		std::stringstream ss1{};
		std::stringstream ss2{};
		ss1<<layer<< " " <<in<< " " << out;
		ss2<<key.layer<< " " <<key.in<< " " << key.out;
		return (ss1.str() < ss2.str());
	}

};


class Neuron {
public:
	Neuron(){}
	void add_in_edge(double e, const Edge_key & k) {edges_in[k] = e;}
	void add_out_edge(double e , const Edge_key & k) {edges_out[k] = e;}
	void set_out(double o) {out = o;}
	double get_out() {return out;}
	double & in_edges_weights(const Edge_key & k) {return edges_in[k];}
	std::vector<Edge_key> in_edges_keys();
	double & out_edges_weights(const Edge_key & k)  {return edges_out[k];}
	std::vector<Edge_key> out_edges_keys();
	double & in_edges_weights(int l, int i, int o) {return edges_in[Edge_key{l,i,o}];}
	double & out_edges_weights(int l, int i, int o)  {return edges_out[Edge_key{l,i,o}];}

private:
	std::map<Edge_key, double> edges_in;
	std::map<Edge_key, double> edges_out;
	double out{1.0};  /*default value for bias!*/
};



#endif /* NEURON_H_ */
