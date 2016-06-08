/*
 * Boosting.cpp
 *
 *  Created on: Jun 1, 2016
 *      Author: marija
 */

#include "Boosting.h"
#include "Dataset.h"
#include <iostream>

using namespace std;

/*
 * This is probably not correct!
 * */
double calculate_true_error(Neural_network & nn, const Data_set & test_set, vector<int> & res_v){
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
		vector<double> res = nn.calculate(inputs[test_idx]);
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

double Boosting::calculate_new_distribution(Neural_network & nn, Data_set & set){
	/*calculate true error*/
	vector<int> res;
	double epsilon = calculate_true_error(nn, set, res);
	double alpha = (epsilon>0.)?0.5*log((1.-epsilon)/epsilon):1000;
	cout << "alpha: " << alpha << endl;
	algorithms.push_back(make_pair(nn, alpha));
	if (int(algorithms.size()) == num_iterations || epsilon == 0.) return epsilon;

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
	return epsilon;
}

void Boosting::operator()(const Data_set &ds) {
	vector<vector<double>> inputs;
	vector<vector<double>> outputs;
	Attribute_normalizer normalizer(ds.attr);
	normalizer.normalized_data(ds,inputs, outputs);
	Data_set tmp = ds; /*a database copy :(*/
	assert(inputs.size()>0);
	int num_inputs = inputs[0].size();
	int num_outputs = outputs[0].size();
	auto out_attrib = tmp.attr.get_attributes_of_kind(Attribute::Attribute_usage::output);
	for (auto attr: out_attrib) {
		auto vals = tmp.attr.get_attr_values(attr);
		assert(vals.size() == 2 && "Boosting algorithm supports only binary output.");
		/*currently algorithm "works" only with binary output. :(*/
	}


	int epoch_count = 2000;
	map<string, string> p;
	p["eta"] = "0.01";
	p["tolerance"] = "0.01";
	p["epoch_count"] = to_string(epoch_count);
	p["config"] = to_string(num_inputs) + " 1 " + to_string(num_outputs);
	Algorithm_parameters params{p};
	for (int i=0; i < num_iterations; i++) {
		Neural_network nn{};
		nn.setup(params);

		/*new learner*/
		Data_set new_set;
		if (i >= 0) {
			tmp.distribute_boosting(new_set);
		} else {
			new_set = tmp;
		}
		//nn(new_set, epoch_count); /*epoch count is 2000.*/
		nn.learn(new_set);
		if (calculate_new_distribution(nn, tmp) == 0.) {
			num_iterations = i+1;
			break;
		}
	}
}

void Boosting::classify(const Data_set & test_set){
	vector<vector<double>> inputs;
	vector<vector<double>> outputs;
	Attribute_normalizer normalizer(test_set.attr);
	normalizer.normalized_data(test_set,inputs, outputs);
	auto attr_outputs = test_set.attr.get_attributes_of_kind(Attribute::Attribute_usage::output);
	int miss_classified = 0;
	for (size_t j=0; j<inputs.size(); j++) {
		vector<double> res(outputs[0].size(), 0.0);
		vector<double> sum(outputs[0].size(), 0.0);
		for (auto nn: algorithms) {
			vector<double> out = nn.first.calculate(inputs[j]);
			for (size_t i=0; i<out.size(); i++){
				/*expand out[i] to {-1, 1}*/
				res[i] += nn.second*(out[i]*2.-1.);
			}
		}
		cout << "res: " << res[0] << endl;
		for (size_t i=0; i<res.size(); i++){
			if (res[i] > 0) {
				res[i] = 1.;
			} else {
				res[i] = 0;
			}
		}
		auto res_labels = normalizer.undo_normalize(attr_outputs, res);
		auto target_labels = normalizer.undo_normalize(attr_outputs, outputs[j]);
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

void boost_tennis_example(int iterations = 5) {
	//Data_set::_test_normalize_columns();

	string file_name = "../data/tennis.txt";
	string class_name = "Play";
	Data_set ds;
	ds.load_simple_db(file_name, class_name);

	Data_set train, test;
	ds.distribute_split(train, test, 0.6);

	Boosting boost{ iterations };
	boost(train);
	boost.classify(test);
}

void boost_zoo_example(int iterations = 5) {
	string file_name = "../data/zoo.txt";
	string class_name = "type";
	Data_set ds;
	ds.load_simple_db(file_name, class_name);

	Data_set train, test;
	ds.distribute_split(train, test, 0.8);

	Boosting boost{ iterations };
	boost(train);
	boost.classify(test);
}

void Boosting::run_examples() {
	boost_tennis_example();

	//boost_zoo_example(); /*cannot work, the output is not binary!*/
}
