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
		cout<< "value: " << p.first << ", " << p.second.get_attr();
		cout <<  ((p.second.is_leaf) ? ": " + *(p.second.get_vals().begin()) : "") << endl;
		p.second.print(depth+1);
	}
}

string Decision_node::classify(const Data & d) {

	if (is_leaf) return *(att_vals.begin());
	string val = d.get_value(attribute);
	return branches[val].classify(d);
}
