/*
 * Dataset.h
 *
 *  Created on: May 5, 2016
 *      Author: marija
 */

#ifndef DATASET_H_
#define DATASET_H_

#include <vector>
#include <map>
#include <set>

class Data {
public:
	Data(const std::vector<std::string>& atts): attributes{atts}{}
	const std::vector<std::string> attributes;
	const std::string & get_value(const std::string & att) const { return values_map.at(att);}
	void set_value(const std::string & att, const std::string & val) {values_map[att] = val;}
	//bool has_attribute(const std::string & att) { auto it = attr_map.find(att); return it != attr_map.end();}
public:
	std::map<std::string, std::string> values_map;
};

struct Attribute{
	std::string name;
	std::string type;
	bool used;
};

class Attribute_set {
public:
	Attribute_set() {}
	const std::vector<std::string> & get_all_attributes() const {return attr_names;}
	std::vector<std::string> get_filtered_attributes() const;
	void append_attr(const Attribute & attr) {
		attr_names.push_back(attr.name);
		attr_types.push_back(attr.type);
		attr_used.push_back(attr.used);
	}
	std::map<std::string, std::set<std::string>> & get_attr_map() {return attr_map;}
	const std::set<std::string> & get_attr_values(const std::string & attr) const;
private:
	std::map<std::string, std::set<std::string>> attr_map;
	std::vector<std::string> attr_names;
	std::vector<std::string> attr_types;
	std::vector<bool> attr_used;
};

class Data_set {
public:
	Data_set(const std::string & l):label_name{l}{}
	void append_data(const Data & d) {data_set.push_back(d);define_attr_values(d);}
	std::vector<int> split_train_set_by_attr_val(const std::vector<int> & subset, const std::string & att, const std::string & val) const;
	//std::vector<int> split_test_set_by_attr_val(const std::string & att, const std::string & val) const;
	const Data & get_elem(int idx) const { return data_set[idx];}
	int get_size() const { return data_set.size();}
	const Data & get_train_elem(int idx) const { return training_set[idx];}
	int get_train_set_size() const { return training_set.size();}

	const Data & get_test_elem(int idx) const { return test_set[idx];}
	int get_test_set_size() const {return test_set.size();}

	const Data_set & init_train_and_test_set(double percentage);
	const std::string & get_label_name() const {return label_name; }
	Attribute_set attr;
private:
	void define_attr_values(const Data & d);
	std::string label_name;
	std::vector<Data> data_set;
	std::vector<Data> training_set;
	std::vector<Data> test_set;
};

#endif /* DATASET_H_ */
