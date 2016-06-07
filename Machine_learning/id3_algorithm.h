#ifndef ID3ALGORITH_H_
#define ID3ALGORITH_H_

#include "Decision_node.h"
#include <algorithm>
#include <vector>

class Data_set;

class id3_algorithm{
public:
	id3_algorithm(Data_set & data);
	Decision_node operator()();

	static void run_examples();
private:
	Decision_node calculate(const std::vector<int> &, const std::vector<std::string> &);
	std::string find_most_common_class(const std::vector<int> & subset);
	double calculate_entropy(const std::vector<int> &);
	double calculate_gain(const std::string & att, const std::vector<int> & subset);
	const Data_set & ds;
	std::string class_name;
	std::set<std::string> class_vals;
};

#endif