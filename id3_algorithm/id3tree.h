/*
 * id3node.h
 *
 *  Created on: May 5, 2016
 *      Author: marija
 */

#ifndef ID3TREE_H_
#define ID3TREE_H_
#include <string>
#include <set>
#include <map>
#include "Dataset.h"

class id3_node {
public:
	id3_node(){}
	id3_node(const std::string & attr, const std::set<std::string> & vals, bool leaf=false): is_leaf{leaf}, attribute{attr}, att_vals{vals}{}
	void add_a_child(const std::string & val, const id3_node & node) {branches[val] = node;}
	void print();
	const std::string & get_attr() const { return attribute;}
	const std::set<std::string> & get_vals() const { return att_vals;}
	const std::map<std::string, id3_node> & get_nodes() const {return branches;}
	bool is_leaf=false;
	std::string classify(const Data & d, const std::string & class_name);
private:
	std::string attribute;
	std::set<std::string> att_vals;
	std::map<std::string, id3_node> branches;
};

#endif /* ID3TREE_H_ */
