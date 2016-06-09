/*
 * Boosting.cpp
 *
 *  Created on: Jun 1, 2016
 *      Author: marija
 */

#include "Boosting.h"
#include "Dataset.h"
#include <iostream>
#include "Neuralnetwork.h"
#include "id3_algorithm.h"
using namespace std;

/*
 * This is probably not correct!
 * */
double Boosting::calculate_true_error(Algorithm & alg, const Data_set & test_set, vector<int> & res_v){
	vector<vector<double>> inputs;
	vector<vector<double>> outputs;
	Attribute_normalizer normalizer(test_set.attr);
	normalizer.normalized_data(test_set,inputs, outputs);
	auto attr_outputs = test_set.attr.get_attributes_of_kind(Attribute::Attribute_usage::output);

	int test_size = test_set.get_size();
	double true_error = 0.0;
	double total_error = 0.0;
	int missclassified = 0;
	for (int test_idx = 0; test_idx < test_size; test_idx++) {
		vector<double> res;
		Data d = data_normalizer.get_unnormalized_data(test_set.attr.get_attributes_of_kind(Attribute::input),
													   inputs[test_idx]);
		alg.classify(d, res);
		vector<double> target = outputs[test_idx];

		auto res_labels = normalizer.undo_normalize(attr_outputs, res);
		auto target_labels = normalizer.undo_normalize(attr_outputs, target);
		int r = 1;
		for (size_t i = 0; i<res.size(); i++) {
			if (res_labels[i].second != target_labels[i].second) {
				r=-1;
				missclassified++;
			}
		}
		true_error +=  (r==-1) ? test_set.get_weight(test_idx): 0.;
		res_v.push_back(r);
	}
	total_error = double(missclassified)/test_size;
	cout << "total error " << total_error << endl;
	cout << "true error " << true_error << endl;
	return true_error;
}

bool Boosting::calculate_new_distribution(Algorithm* alg, Data_set & set){
	/*calculate true error*/
	vector<int> res;
	double epsilon = calculate_true_error(*alg, set, res);
	if (epsilon - 0.5 >= 0.) throw runtime_error("Weak learner is not good enough!.");
	epsilon = (epsilon > 0.0)? epsilon: 0.0001;
	double alpha = 0.5*log((1.-epsilon)/epsilon);
	cout << "alpha: " << alpha << endl;
	shared_ptr<Algorithm> p{alg};
	algorithms.push_back(make_pair(p, alpha));
	num_algorithms++;
	if (epsilon < 0.0001) return true;
	/*calculate new weights*/
	vector<pair<int, double>> new_weights;
	double sum = 0.0;
	for (const auto & w:set.get_weights()) {
		double new_w = w.second*exp(alpha*res[w.first]);
		sum += new_w;
		new_weights.push_back(make_pair(w.first,new_w));
	}
	for (size_t i=0; i<new_weights.size(); i++) {
		new_weights[i].second /= sum;
	}
	set.set_weights(new_weights);
	return false;
}

void Boosting::learn(const Data_set &ds) {
	vector<vector<double>> inputs;
	vector<vector<double>> outputs;
	data_normalizer = Attribute_normalizer{ds.attr};
	data_normalizer.normalized_data(ds,inputs, outputs);
	assert(inputs.size()>0);
	common(ds);
}

void Boosting::learn(const Data_set &ds, const  Attribute_normalizer & a_nom) {
	vector<vector<double>> inputs;
	vector<vector<double>> outputs;
	data_normalizer = a_nom;
	data_normalizer.normalized_data(ds,inputs, outputs);
	assert(inputs.size()>0);
	common(ds);
}

void Boosting::common(const Data_set &ds){
	Data_set tmp = ds; /*a database copy :(*/
	Data_set new_set = tmp; /*another :(*/

	auto out_attrib = tmp.attr.get_attributes_of_kind(Attribute::Attribute_usage::output);
	for (auto attr: out_attrib) {
		auto vals = tmp.attr.get_attr_values(attr);
		assert(vals.size() == 2 && "Boosting algorithm supports only binary output.");
		/*currently algorithm "works" only with binary output. :(*/
	}
	for (const auto & p: alg_params) {
		Algorithm * alg;
		string s = p.get("algorithm");
		if( s  == "neu_net" ){
			alg = new Neural_network{};
		} else if ( s == "id3") {
			alg = new id3_algorithm{};
		} else {
			throw runtime_error("Requested algorithm not supported for Boosting.");
		}

		alg->setup(p);

		/*new learner*/
		alg->learn(new_set);
		if (calculate_new_distribution(alg, tmp)) {
			break;
		}
		tmp.distribute_boosting(new_set);
	}
}

void Boosting::classify(const Data & d, vector<double> & out) {
	for (auto alg: algorithms) {
		alg.first->classify(d, out);
		for (size_t i=0; i<out.size(); i++){
			/*expand out[i] to {-1, 1}*/
			out[i] += (out[i]*2.-1.) * alg.second;
		}
	}
}

void Boosting::classify_all(const Data_set & test_set){
	vector<vector<double>> inputs;
	vector<vector<double>> outputs;
	data_normalizer.normalized_data(test_set,inputs, outputs);
	auto attr_outputs = test_set.attr.get_attributes_of_kind(Attribute::Attribute_usage::output);
	int miss_classified = 0;
	for (size_t j=0; j<inputs.size(); j++) {
		vector<double> res(outputs[0].size(), 0.0);
		vector<double> sum(outputs[0].size(), 0.0);
		Data d = data_normalizer.get_unnormalized_data(test_set.attr.get_attributes_of_kind(Attribute::input), inputs[j]);
		for (auto alg: algorithms) {
			vector<double> out(outputs[0].size(), 0.0);
			alg.first->classify(d, out);
			for (size_t i=0; i<out.size(); i++){
				/*expand out[i] to {-1, 1}*/
				res[i] += (out[i]*2.-1.) * alg.second;
			}
		}
		for (size_t i=0; i<res.size(); i++){
			if (res[i] > 0) {
				res[i] = 1.;
			} else {
				res[i] = 0;
			}
		}
		auto res_labels = data_normalizer.undo_normalize(attr_outputs, res);
		auto target_labels = data_normalizer.undo_normalize(attr_outputs, outputs[j]);
		for (size_t i=0; i<res.size(); i++){
			if (res_labels[i] != target_labels[i] ) {
				miss_classified++;
			}
		}

	}
	cout << miss_classified << " missclassified out of " << outputs.size() << endl;
	cout << "Total error: " << double(miss_classified)/outputs.size() << endl;
}

/*******************
 * E X A M P L E S *
 *******************/

void boost_tennis_example(int iterations = 10) {
	//Data_set::_test_normalize_columns();

	string file_name = "../data/tennis.txt";
	string class_name = "Play";
	Data_set ds;
	ds.load_simple_db(file_name, class_name);

	Data_set train, test;
	ds.distribute_split(train, test, 0.6, true);
	vector<Algorithm_parameters> attr;
	map<string, string> p;

	p["algorithm"] = "neu_net";
	p["eta"] = "0.2";
	p["tolerance"] = "0.01";
	p["epoch_count"] = "1000";
	p["config"] = " 4 1 1 ";
	/**//*
	p["algorithm"] ="id3";
	p["depth"] = "1"; /**/
	for (int i=0; i<iterations; i++) {
		attr.push_back(Algorithm_parameters{p});
	}
	Boosting boost{attr};
	boost.learn(train);
	boost.classify_all(test);
}

void boost_zoo_example(int iterations = 5) {
	string file_name = "../data/zoo.txt";
	string class_name = "type";
	Data_set ds;
	ds.load_simple_db(file_name, class_name);

	Data_set train, test;
	ds.distribute_split(train, test, 0.8);
	vector<Algorithm_parameters> attr;
	map<string, string> p;
	p["algorithm"] = "neu_net";
	p["eta"] = "0.01";
	p["tolerance"] = "0.01";
	p["epoch_count"] = "1000";
	p["config"] = " 16 1 1 ";
	for (int i=0; i<iterations; i++) {
		attr.push_back(Algorithm_parameters{p});
	}
	Boosting boost{attr};
	boost.learn(train);
	boost.classify_all(test);
}

void Boosting::run_examples() {
	boost_tennis_example(10);
}
