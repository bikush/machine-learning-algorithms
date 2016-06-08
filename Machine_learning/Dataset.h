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
#include <assert.h>
#include <math.h>

class Data {
public:
	Data(const std::vector<std::string>& atts): attributes{atts}{}
	std::vector<std::string> get_attributes() {return attributes;}
	const std::string & get_value(const std::string & att) const { return values_map.at(att);}
	void set_value(const std::string & att, const std::string & val) {values_map[att] = val;}
	void print() const;

	//bool has_attribute(const std::string & att) { auto it = attr_map.find(att); return it != attr_map.end();}
private:
	std::map<std::string, std::string> values_map;
	std::vector<std::string> attributes;

	friend std::istream & operator>>(std::istream & is, Data & d);
};

struct Attribute{
	enum Attribute_usage {
		none = 0,
		input = 1,
		output = 2
	};
	std::string name;
	std::string type;
	Attribute_usage usage;
};

class Attribute_set;
class Data_set;

class Attribute_normalizer {
public:
	Attribute_normalizer() {};
	Attribute_normalizer(const Attribute_set& attributes);
	void add_attribute( const std::string & attr_name, const std::set<std::string> & values);
	void normalize(const Data & data, const std::vector<std::string> & attributes, std::vector<double>& output) const;
	std::vector<std::pair<double,std::string>> undo_normalize(const std::vector<std::string> & attributes, const std::vector<double> & value);
	void reset() { transform.clear(); };

	void normalized_data(const Data_set& data, std::vector<std::vector<double>> & inputs, std::vector<std::vector<double>> & outputs) const;
	void normalized_data_columns(const Data_set& data, std::vector<std::vector<double>> & inputs, std::vector<std::vector<double>> & outputs) const;
	Data get_unnormalized_data(const std::vector<std::string>& attributes, const std::vector<double>& values);

private:
	std::map<std::string, std::map<std::string, double>> transform;
	std::map<std::string, double> span;
};

class Attribute_set {
public:
	Attribute_set() {}
	const std::vector<std::string> & get_all_attributes() const {return attr_names;}
	std::vector<std::string> get_attributes_of_kind(Attribute::Attribute_usage usage) const;

	void append_attr(const Attribute & attr) {
		attr_names.push_back(attr.name);
		attr_types.push_back(attr.type);
		attr_usage.push_back(attr.usage);
	}
	std::map<std::string, std::set<std::string>> & get_attr_map() {return attr_map;}
	const std::set<std::string> & get_attr_values(const std::string & attr) const;
	const int count_attr_by_usage(Attribute::Attribute_usage usage) const;
	//const Attribute_normalizer& get_normalizer() const { return normalizer; };
	//void generate_normalizer();

private:
	std::map<std::string, std::set<std::string>> attr_map;
	std::vector<std::string> attr_names;
	std::vector<std::string> attr_types;
	std::vector<Attribute::Attribute_usage> attr_usage;
	//Attribute_normalizer normalizer;
};

class Data_set {
public:
	Data_set(const std::string & l = "");

	void load_simple_db(const std::string & path);
	void load_simple_db(const std::string & path, const std::string & class_name);

	void append_data(const Data & d) {data_set.push_back(d);define_attr_values(d);}
	std::vector<int> split_by_attr_val(const std::vector<int> & subset, const std::string & att, const std::string & val) const;
	
	const Data & get_elem(int idx) const { return data_set[idx];}
	int get_size() const { return data_set.size();}

	const std::string & get_label_name() const {return label_name; }
	Attribute_set attr;

	void shuffle_data();

	void fill_subset(Data_set & subset, const std::vector<int> & subset_indice) const;
	void distribute_split(Data_set & first, Data_set & second, double percentage, bool random = false) const;
	void distribute_fold(Data_set & first, Data_set & second, int fold_count, int take_fold) const;
	void distribute_boosting(Data_set &);
	std::vector<std::pair<int, double>> get_weights() const { return weights; }
	double get_weight(int idx) const;
	void set_weights(const std::vector<std::pair<int, double>> & new_w);
	
	static void _test_normalize();
	static void _test_normalize_columns();
	static void _test_distribute_fold();

	static Data_set load_tennis();
	static Data_set load_zoo();

	double measure_error(const Data& data, const std::string& guessed_class) const;
	double measure_error(const Data& data, double guessed_class) const;
	
private:
	void define_attr_values(const Data & d);
	void clear();
	void set_init_weights();
	std::string label_name;
	std::vector<Data> data_set;
	std::vector<std::pair<int, double>> weights;
};

#endif /* DATASET_H_ */
