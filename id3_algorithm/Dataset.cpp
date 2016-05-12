/*
 * Dataset.cpp
 *
 *  Created on: May 5, 2016
 *      Author: marija
 */

#include "Dataset.h"

using namespace std;

const Data_set & Data_set::init_train_and_test_set(double percentage) {
	int train_size = percentage * get_size();
	training_set.clear();
	test_set.clear();
	for (int i=0; i<get_size(); i++) {
		if (i<train_size) {
			training_set.push_back(data_set[i]);
		} else {
			test_set.push_back(data_set[i]);
		}
	}
	return *(this);
}

vector<int> Data_set::split_train_set_by_attr_val(const vector<int> & subset, const string & att, const string & val) const {
	vector<int> idxs;
	for (size_t i=0; i<subset.size(); i++) {
		int idx = subset[i];
		if (training_set[idx].get_value(att) == val) {
			idxs.push_back(idx);
		}
	}
	return idxs;
}

void Data_set::define_attr_values(const Data & d){
	for (const auto & att: attr.get_all_attributes()) {
		auto it = attr.get_attr_map().find(att);
		string val = d.get_value(att);
		if (it == attr.get_attr_map().end()) {
			attr.get_attr_map()[att] = set<string>{{val}};
		} else {
			attr.get_attr_map()[att].insert(val);
		}
	}
}

vector<string> Attribute_set::get_filtered_attributes() const {
	vector<string> res;
	for (size_t i=0; i<attr_names.size(); i++) {
		if (attr_used[i])
			res.push_back(attr_names[i]);
	}
	return res;
}


const set<string> & Attribute_set::get_attr_values(const string & attr) const{
	return attr_map.at(attr);
}
