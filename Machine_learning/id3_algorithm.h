#ifndef ID3ALGORITH_H_
#define ID3ALGORITH_H_

#include "Decision_node.h"
#include <algorithm>
#include <vector>
#include "Algorithm.h"
#include "Dataset.h"

class id3_algorithm : public Algorithm<Decision_node, std::string>{
public:
	id3_algorithm();
	
	void setup(const Algorithm_parameters& parameters);
	Decision_node learn(const Data_set & data_set);

	static void run_examples();
private:
	Decision_node calculate(const std::vector<int> &, const std::vector<std::string> &);
	std::string find_most_common_class(const std::vector<int> & subset);
	double calculate_entropy(const std::vector<int> &);
	double calculate_gain(const std::string & att, const std::vector<int> & subset);
	
	// TODO: id3 algo is suboptimal with copying the data_set at every learning phase
	Data_set ds;
	std::string class_name;
	std::set<std::string> class_vals;
};

#endif
