#ifndef ID3ALGORITH_H_
#define ID3ALGORITH_H_

#include "Decision_node.h"
#include <algorithm>
#include <vector>
#include "Algorithm.h"
#include "Dataset.h"

class id3_algorithm : public Algorithm{
public:
	id3_algorithm(int depth = 0);
	
	void classify(const Data & d, double & out);
	void setup(const Algorithm_parameters& parameters);
	void learn(const Data_set & data_set);

	void set_max_depth(int depth);
	const Decision_node& get_node() const { return node; }

	static void run_examples();
private:
	Decision_node calculate(const std::vector<int> &, const std::vector<std::string> &, int);
	std::string find_most_common_class(const std::vector<int> & subset);
	double calculate_entropy(const std::vector<int> &);
	double calculate_gain(const std::string & att, const std::vector<int> & subset);
	
	// TODO: id3 algo is suboptimal with copying the data_set at every learning phase
	Data_set ds;
	int max_depth;
	std::string class_name;
	std::set<std::string> class_vals;
	Decision_node node;
};

#endif
