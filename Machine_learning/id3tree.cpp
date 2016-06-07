/*
 * id3tree.cpp
 *
 *  Created on: May 5, 2016
 *      Author: marija
 */

#include "id3tree.h"
#include <iostream>
using namespace std;

void print_rec(const id3_node & tmp, int depth) {
	string pre = "";
	for (int i=0; i<depth; i++) {
		pre += "  ";
	}
	if (tmp.is_leaf) {
		return;
	}
	cout << pre;
	cout << depth << " Deciding on -> " << tmp.get_attr()<<endl;

	for (auto p: tmp.get_nodes()) {
		cout << pre << pre;
		cout<< "value: " << p.first << ", " << p.second.get_attr();
		cout <<  ((p.second.is_leaf) ? ": " + *(p.second.get_vals().begin()) : "") << endl;
		print_rec(p.second, depth+1);
	}
}

void id3_node::print() {
	print_rec(*(this), 0);
}

string id3_node::classify(const Data & d, const string & class_name) {

	if (attribute == class_name) return *(att_vals.begin());
	string val = d.get_value(attribute);
	return branches[val].classify(d, class_name);
}
