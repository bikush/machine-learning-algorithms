/*
 * Dataset.cpp
 *
 *  Created on: May 5, 2016
 *      Author: marija
 */

#include "Dataset.h"
#include <algorithm>
#include <iostream>

using namespace std;

void Data_set::distribute_split(Data_set & first, Data_set & second, double percentage)
{
	percentage = std::min(1.0, std::max(0.0, percentage));
	first.attr = attr;
	first.label_name = label_name;
	second.attr = attr;
	second.label_name = label_name;
	int total_size = get_size();
	int first_size = int(percentage * total_size);
	for (int i = 0; i < first_size; i++) {
		first.data_set.push_back(data_set[i]);
	}
	for (int i = first_size; i < total_size; i++) {
		second.data_set.push_back(data_set[i]);
	}	
}

void Data_set::distribute_random(Data_set & first, Data_set & second, double percentage)
{
}

void Data_set::distribute_fold(Data_set & first, Data_set & second, int fold_count, int take_fold)
{
}

vector<int> Data_set::split_by_attr_val(const vector<int> & subset, const string & att, const string & val) const {
	vector<int> idxs;
	for (size_t i=0; i<subset.size(); i++) {
		int idx = subset[i];
		// TODO: valid idx?
		if (data_set[idx].get_value(att) == val) {
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

void Data_set::clear()
{
	label_name = "";
	data_set.clear();
	attr = Attribute_set{};
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
