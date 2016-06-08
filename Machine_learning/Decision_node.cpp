/*
 * Decision_node.cpp
 *
 *  Created on: May 5, 2016
 *      Author: marija
 *  Renamed on: June 7, 2016
 *
 */

#include "Decision_node.h"
#include <iostream>
using namespace std;

void Decision_node::print(int depth) {
	string pre = "";
	for (int i=0; i<depth; i++) {
		pre += "  ";
	}
	if (is_leaf) {
		return;
	}
	cout << pre;
	cout << depth << " Deciding on -> " << get_attr()<<endl;

	for (auto p: get_nodes()) {
		cout << pre << pre;
		cout<< "  value: " << p.first << ", " << p.second.get_attr();
		cout << ((p.second.is_leaf) ? (": " + p.second.leaf_val) : "") << endl;
		p.second.print(depth+1);
	}
}

string Decision_node::classify(const Data & d) {

	if (is_leaf) return leaf_val;
	string val = d.get_value(attribute);
	return branches[val].classify(d);
}

void Decision_node::classify(const Data & d, std::vector<double> & out)
{
	auto result = classify(d);
	//out = d.normalize(result);
	// TODO: solve this situation
	out.push_back( 1.0 );
}

void Decision_node::setup(const Algorithm_parameters & parameters)
{
	// Nothing
}

void Decision_node::learn(const Data_set & data_set, const Attribute_normalizer& normalizer)
{
	// Use id3 class
}
