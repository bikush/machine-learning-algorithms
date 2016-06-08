/*
 * Algorithm.cpp
 *
 *  Created on: Jun 7, 2016
 *      Author: marija
 */

#include "Algorithm.h"
#include <algorithm>
#include "Dataset.h"

using namespace std;

string Algorithm_parameters::get(const string& name) const {
	return safe_get(name, "");
}

int Algorithm_parameters::get_int(const string& name) const {
	return stoi(safe_get(name, "0"));
}

double Algorithm_parameters::get_double(const string& name) const {
	return stod(safe_get(name, "0.0"));
}

string Algorithm_parameters::safe_get(const string& name, const string& backup) const {
	auto found_it = parameters.find(name);
	if (found_it == parameters.end()) {
		return backup;
	}
	else {
		return found_it->second;
	}
}

void Algorithm::learn(const Data_set & data_set)
{
	Attribute_normalizer normalizer(data_set.attr);
	learn(data_set, normalizer);
}