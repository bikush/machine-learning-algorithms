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
	void print();

	//bool has_attribute(const std::string & att) { auto it = attr_map.find(att); return it != attr_map.end();}
private:
	std::map<std::string, std::string> values_map;
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

class Attribute_normalizer {
public:
	void add_attribute( const std::string & attr_name, const std::set<std::string> & values);
	void normalize(const Data & data, const std::vector<std::string> & attributes, std::vector<double>& output);
	std::vector<std::pair<double,std::string>> undo_normalize(const std::vector<std::string> & attributes, const std::vector<double> & value);
	void reset() { transform.clear(); };
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
	const Attribute_normalizer& get_normalizer() const { return normalizer; };
	void generate_normalizer();

private:
	std::map<std::string, std::set<std::string>> attr_map;
	std::vector<std::string> attr_names;
	std::vector<std::string> attr_types;
	std::vector<Attribute::Attribute_usage> attr_usage;
	Attribute_normalizer normalizer;
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

	void distribute_split(Data_set & first, Data_set & second, double percentage);
	void distribute_random(Data_set & first, Data_set & second, double percentage);
	void distribute_fold(Data_set & first, Data_set & second, int fold_count, int take_fold);
	// * * * * * * * *
	// f_c = 4, t_f = 1
	// f f|s s|f f|f f

	void normalized_data( std::vector<std::vector<double>> & inputs, std::vector<std::vector<double>> & outputs ) const;
	void normalized_data_columns(std::vector<std::vector<double>> & inputs, std::vector<std::vector<double>> & outputs) const;

	static void _test_normalize();
	static void _test_normalize_columns();
	
private:
	void define_attr_values(const Data & d);
	void clear();
	std::string label_name;
	std::vector<Data> data_set;
};

#endif /* DATASET_H_ */
