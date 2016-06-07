#pragma once

#include <map>
#include <string>
#include <chrono>
#include <iostream>
#include <algorithm>
#include "Dataset.h"
#include "Algorithm.h"
#include "id3_algorithm.h"

class Spoof_classifier:public Classifier<double> {
public:
	Spoof_classifier(double f=1.): error{f}{}
	Spoof_classifier(Spoof_classifier && sc) {error = sc.error;}
	void operator=(Spoof_classifier && sc) {error = sc.error;}

	Spoof_classifier(const Spoof_classifier & sc) {error = sc.error;}
	void operator=(const Spoof_classifier & sc) {error = sc.error;}

    void classify(const Data & d, double & out) {
		int seed = (int)std::chrono::system_clock::now().time_since_epoch().count();
		out = std::min( std::max( error + ((seed % 100)-50) / 1000.0, 0.0 ), 1.0);
    }
private:
	double error;
	Data_set xd;
};

class Spoof_la : public Algorithm<Spoof_classifier, double> {
private:
	Algorithm_parameters parameters;

public:
	Spoof_la():Algorithm{} {
	}

	virtual void setup(const Algorithm_parameters& param) {
		parameters = param;
	}

	virtual Spoof_classifier learn(const Data_set & data_set) {
		return Spoof_classifier(parameters.get_double("error"));
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

	template <class T>
	static std::vector<double> k_fold(const Data_set& data_set, T & algorithm, int k) {
		k = k < 2 ? 2 : (k > data_set.get_size() ? data_set.get_size() : k);
		Data_set train, validate;
		std::vector<double> errors(k);
		for (int fold = 0; fold < k; fold++) {
			data_set.distribute_fold(train, validate, k, fold);
			auto classifier = algorithm.learn(train);
			double avg_error = 0.0;
			for (int val_idx = 0; val_idx < validate.get_size(); val_idx++) {
				auto data = validate.get_elem(val_idx);
				T::class_type class_guess;
				classifier.classify( data, class_guess);
				double error = data_set.measure_error(data, class_guess);
				avg_error += error;
			}
			avg_error /= validate.get_size();
			errors[fold] = avg_error;
		}
		return errors;
	}

	static void test() {
		//Spoof_la spoof;
		// TODO: fix the example?
		Algorithm_parameters params{ {std::pair<std::string,std::string>("error","0.15")} };
		Data_set data("data1");
		Data fake_data{ {"data1","data2"} };
		fake_data.set_value("data1", "one");
		fake_data.set_value("data2", "two");
		for (int i = 0; i < 500; i++) {
			data.append_data(fake_data);
		}
		data.attr.generate_normalizer();
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

	static void id3_test() {
		Algorithm_parameters params{};
		Data_set data = Data_set::load_zoo();
		data.shuffle_data();

		for (int i = 10; i > 1; i--) {
			std::cout << "K: " << i;
			auto result = k_fold_t<id3_algorithm>(data, params, i);
			double error = 0.0;
			for (auto res : result) {
				error += res;
			}
			error /= result.size();
			std::cout << ", error: " << error << std::endl;
		}
	}

	static void run_examples() {
		//test();
		id3_test();
	}
};
