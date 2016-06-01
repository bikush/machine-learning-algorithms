#pragma once

#include <map>
#include <string>
#include <chrono>
#include <iostream>
#include <algorithm>
#include "Dataset.h"

class Algorithm_parameters {
public:
	typedef std::map<std::string, std::string> param_map;
	Algorithm_parameters(const param_map& params = {}) : parameters(params) {};

	std::string get(const std::string& name) {
		return safe_get(name, "");
	}

	int get_int(const std::string& name) {
		return std::stoi(safe_get(name, "0"));
	}

	double get_double(const std::string& name) {
		return std::stod(safe_get(name, "0.0"));
	}

	std::string safe_get(const std::string& name, const std::string& backup) const {
		auto found_it = parameters.find(name);
		if (found_it == parameters.end()) {
			return backup;
		}
		else {
			return found_it->second;
		}
	}

private:
	param_map parameters;
};

class Learning_algorithm {
public:
	//virtual ~Learning_algorithm() = 0;
	virtual void setup(const Algorithm_parameters& parameters) = 0;
	virtual void reload() = 0;
	virtual void learn(const Data_set & data_set) = 0;
	virtual double classify(const Data& data) = 0;
};

class Spoof_la : public Learning_algorithm {
private:
	Algorithm_parameters parameters;
	double error;

public:
	Spoof_la() {
	}

	virtual void setup(const Algorithm_parameters& param) {
		parameters = param;
	}

	virtual void reload() {
		//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		//error = 0.5 + (seed % 1000) / 2000.0;

		error = parameters.get_double("error");
	}

	virtual void learn(const Data_set & data_set) {
		// Spoof
	}

	virtual double classify(const Data& data) {
		int seed = (int)std::chrono::system_clock::now().time_since_epoch().count();
		return std::min( std::max( error + ((seed % 100)-50) / 1000.0, 0.0 ), 1.0);
	}
};

class Cross_validation
{
public:

	template<class T>
	static std::vector<double> k_fold_t(const Data_set& data_set, const Algorithm_parameters& param, int k) {
		T algorithm;
		algorithm.setup(param);
		return k_fold(data_set, algorithm, k);
	}

	static std::vector<double> k_fold(const Data_set& data_set, Learning_algorithm& algorithm, int k) {
		k = k < 2 ? 2 : (k > data_set.get_size() ? data_set.get_size() : k);
		Data_set train, validate;
		std::vector<double> errors(k);
		for (int fold = 0; fold < k; fold++) {
			data_set.distribute_fold(train, validate, k, fold);
			algorithm.reload();
			algorithm.learn(train);
			double avg_error = 0.0;
			for (int val_idx = 0; val_idx < validate.get_size(); val_idx++) {
				double error = algorithm.classify( validate.get_elem(val_idx) );
				avg_error += error;
			}
			avg_error /= validate.get_size();
			errors[fold] = avg_error;
		}
		return errors;
	}

	static void test() {
		//Spoof_la spoof;
		Algorithm_parameters params{ {std::pair<std::string,std::string>("error","0.15")} };
		Data_set data;
		Data fake_data{ {"data1","data2"} };
		fake_data.set_value("data1", "one");
		fake_data.set_value("data2", "two");
		for (int i = 0; i < 500; i++) {
			data.append_data(fake_data);
		}
		//spoof.setup(params);
		//auto result = k_fold(data, spoof, 10);
		for (int i = 10; i > 1; i--) {
			auto result = k_fold_t<Spoof_la>(data, params, 10);
			double error = 0.0;
			for (auto res : result) {
				error += res;
			}
			error /= result.size();
			std::cout << "K:" << i << ", error: " << error << std::endl;
		}

		/*auto result = k_fold_t<Spoof_la>(data, params, 10);
		int idx = 0;
		for (auto res : result) {
			std::cout << idx << ": " << res << std::endl;
			idx++;
		}*/
	}
};

