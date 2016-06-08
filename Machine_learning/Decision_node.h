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
#include "Algorithm.h"

class Decision_node : public Algorithm {
public:
	Decision_node(){}
	Decision_node(const std::string & attr, bool leaf=false, const std::string& leaf_value = ""):
		attribute{attr}, is_leaf{leaf}, leaf_val(leaf_value){}
	void add_a_child(const std::string & val, const Decision_node & node) {branches[val] = node;}
	void print( int depth = 0 );
	std::string classify(const Data & d);

	// Classifier<string>
	void classify(const Data & d, double & out);
	void setup(const Algorithm_parameters& parameters);
	void learn(const Data_set & data_set, const Attribute_normalizer& normalizer);

private:
	std::string attribute;
	std::map<std::string, Decision_node> branches;
	bool is_leaf = false;
	std::string leaf_val = "";

	const std::string & get_attr() const { return attribute; }
	const std::map<std::string, Decision_node> & get_nodes() const { return branches; }

};
/*
class Decision_leaf : public Decision_node {
public:
	Decision_leaf( const std::string& value ) : leaf_value(value) {}
	virtual std::string classify(const Data & d, const std::string & class_name) {
		return leaf_value;
	}

private:
	std::string leaf_value;
};

class Decision_branch : public Decision_node {
public:
	virtual std::string classify(const Data & d, const std::string & class_name) {
		std::string val = d.get_value(attribute);
		return branches[val].classify(d, class_name);
	}
private:
	std::string attribute;
	std::set<std::string> att_vals;
	std::map<std::string, Decision_node> branches;
};*/

#endif /* ID3TREE_H_ */
