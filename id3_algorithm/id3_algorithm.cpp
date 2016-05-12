#include "id3_algorithm.h"
#include <iostream>
using namespace std;

id3_algorithm::id3_algorithm(Data_set & data, double p): ds{data.init_train_and_test_set(p)} {
	class_name = ds.get_label_name();
	class_vals = ds.attr.get_attr_values(class_name);
}

id3_algorithm::id3_algorithm(Data_set & data, bool unused) : ds{ data }
{
	class_name = ds.get_label_name();
	class_vals = ds.attr.get_attr_values(class_name);
}

double id3_algorithm::calculate_entropy(const vector<int> & subset) {
	set<string> vals = ds.attr.get_attr_values(class_name);
	double res = 0.0;
	for (const auto &val: vals) {
		vector<int> indexes = ds.split_train_set_by_attr_val(subset, class_name, val);
		if( indexes.size() == 0 )
			continue;

		double p = double(indexes.size())/double(subset.size());
		res -= p*log2(p);
	}
	return res;
}

double id3_algorithm::calculate_gain(const string & att, const vector<int> & subset) {
	set<string> vals = ds.attr.get_attr_values(att);
	double res = 0.0;
	for (const auto &val: vals) {
		vector<int> indexes = ds.split_train_set_by_attr_val(subset, att, val);
		if( indexes.size() == 0 )
			continue;

		double p = double(indexes.size())/ds.get_train_set_size();
		res += p * calculate_entropy(indexes);
	}
	cout << att << " gain " << res << endl;
	return res;
}

string id3_algorithm::find_most_common_class(const vector<int> & subset) {
	int max = 0;
	string common;
	for (const auto & val: class_vals) {
		int num = 0;
		for (int i: subset) {
			if (ds.get_train_elem(i).get_value(class_name) == val) {
				num += 1;
			}
		}
		if (max < num) {
			max = num;
			common = val;
		}
	}
	return common;
}

id3_node id3_algorithm::operator()() {
	vector<int> subset;
	subset.reserve(ds.get_train_set_size());
	for (int i=0; i< ds.get_train_set_size(); i++) {
		subset.push_back(i);
	}
	return calculate(subset, ds.attr.get_filtered_attributes());
}


id3_node id3_algorithm::calculate(const vector<int> & subset, const vector<string> & attributes) {
	for (const auto & val: class_vals) {
		bool all_the_same= true;
		for (int i: subset) {
			if (ds.get_train_elem(i).get_value(class_name) !=val) {
				all_the_same = false;
				break;
			}
		}
		if (all_the_same) {
			return id3_node{class_name, set<string>{val}, true};
		}
	}


    // check if the are any more attributes:
	if (attributes.size() == 0) {
		return id3_node{class_name, set<string>{find_most_common_class(subset)}, true};
	}

	double min = calculate_gain(attributes[0], subset);
	string attribute = attributes[0];
	for (const auto & att: attributes) {
		// calculate info gain for curr
		// find min, choose that attr
		double et = calculate_gain(att, subset);
		if (et < min) {
			min = et;
			attribute = att;
		}
	}
	cout << "I select: " << attribute << endl;
	// remove selected attribute
	vector<string> attr;
	for (const auto & at: attributes) {
		if (at != attribute)
			attr.push_back(at);
	}
	// do the splitting
	auto vals = ds.attr.get_attr_values(attribute);
	id3_node new_node{attribute, vals};
	for (const auto & v : vals) {
		vector<int> res = ds.split_train_set_by_attr_val(subset, attribute, v);

		if (res.size()) {
			new_node.add_a_child(v, calculate(res, attr));
		} else {
			// no val of this type found.
			new_node.add_a_child(v, id3_node{class_name, set<string>{find_most_common_class(subset)}, true});
		}
	}
	return new_node;
}
