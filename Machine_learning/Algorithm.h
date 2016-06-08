/*
 * Algorithm.h
 *
 *  Created on: Jun 7, 2016
 *      Author: marija
 */

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <map>
#include <string>
#include <memory>
#include <type_traits>

class Data_set;
class Data;

class Algorithm_parameters {
public:
	typedef std::map<std::string, std::string> param_map;
	Algorithm_parameters(const param_map& params = {}) : parameters(params) {};

	std::string get(const std::string& name) const;

	int get_int(const std::string& name) const;

	double get_double(const std::string& name) const;

	std::string safe_get(const std::string& name, const std::string& backup) const;

private:
	param_map parameters;
};

template<class T>
class Classifier {
public:
	virtual ~Classifier(){};
	// stump until ready to remove
};

class Algorithm {
public:
	virtual ~Algorithm(){ };
	virtual void setup(const Algorithm_parameters& parameters) = 0;
	virtual void learn(const Data_set & data_set) = 0;
	virtual void classify(const Data & d, double & out) = 0;
};

#endif /* ALGORITHM_H_ */
