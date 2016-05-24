#define _USE_MATH_DEFINES
#include <iostream>
#include <algorithm>
#include "Dataset.h"
#include "Neuralnetwork.h"
using namespace std;

void transform_values(vector<double> & values, double offset, double scale) {
	for (size_t i = 0; i < values.size(); i++) {
		values[i] = values[i] * scale + offset;
	}
}

void generate_sin(vector<double> & values, vector<double> & res) {
	size_t size = values.size();
	for (size_t i = 0; i < size; i++) {
		values[i] = i*M_PI*2.0 / size;
		res[i] = sin(values[i]);
	}
}

void sine_example( int iterations = 1000) {
	vector<int> init{ 1, 3, 1 };
	vector<vector<double>> inputs(1);
	vector<vector<double>> outputs(1);
	Neural_network nn{ init, 0.6 };

	inputs[0] = vector<double>(50);
	outputs[0] = vector<double>(50);
	generate_sin(inputs[0], outputs[0]);
	transform_values(inputs[0], 0.0, 1 / (M_PI*2.0));
	transform_values(outputs[0], 0.5, 0.5);

	nn(inputs, outputs, iterations);

	vector<vector<double>> t_inputs(1);
	vector<vector<double>> t_outputs(1);
	t_inputs[0] = vector<double>(200);
	t_outputs[0] = vector<double>(200);
	generate_sin(t_inputs[0], t_outputs[0]);
	transform_values(t_inputs[0], 0.0, 1 / (M_PI*2.0));
	transform_values(t_outputs[0], 0.5, 0.5);

	double total_error = 0.0;
	double min_error = 1.0;
	double max_error = 0.0;
	for (size_t i = 0; i < t_inputs[0].size(); i++) {
		double error = Neural_network::test(nn, vector<double>{t_inputs[0][i]}, vector<double>{t_outputs[0][i]});
		total_error += error;
		min_error = min(min_error, error);
		max_error = max(max_error, error);
	}
	total_error /= t_inputs[0].size();
	cout << "total error: " << total_error << endl;
	cout << "min error: " << min_error << endl;
	cout << "max error: " << max_error << endl;
	cout << "avg span: " << (max_error - min_error) / 2.0 << endl;
}

void xor_example( int iterations = 1000 ) {
	vector<int> init{ 2, 3, 1 };
	vector<vector<double>> inputs(2);
	vector<vector<double>> outputs(1);
	inputs[0] = vector<double>{ 0, 0, 1, 1 };
	inputs[1] = vector<double>{ 0, 1, 0, 1 };
	outputs[0] = vector<double>{ 0, 1, 1, 0 };
	Neural_network nn{ init, 0.1 };
	nn(inputs, outputs, iterations);

	Neural_network::test(nn, vector<double>{1, 1}, vector<double>{0});
	Neural_network::test(nn, vector<double>{1., 0}, vector<double>{1});
	Neural_network::test(nn, vector<double>{0, 1.}, vector<double>{1});
	Neural_network::test(nn, vector<double>{0, 0}, vector<double>{0});
}

void load_dataset() {
	string file_name = "../data/tennis.txt";
	string class_name = "Play";  // type
	string name = "Day";
	Data_set ds;
	ds.load_simple_db(file_name, class_name);

	auto all_attr = ds.attr.get_all_attributes();
	for (auto attr : all_attr) {
		cout << attr << ": ";
		auto att_vals = ds.attr.get_attr_values(attr);
		for (auto val : att_vals) {
			cout << val << ", ";
		}
		cout << "increment: " << 1.0 / (att_vals.size() - 1) << endl;
	}
	cout << endl;

	vector<vector<double>> inputs, outputs;
	ds.normalized_data(inputs,outputs);
	auto input_attrs = ds.attr.get_attributes_of_kind(Attribute::Attribute_usage::input);
	for (int i = 0; i < inputs.size(); i++) {
		Data data = ds.get_elem(i);
		data.print();
		auto& elem = inputs[i];
		for (auto val : elem) {
			cout << val << ", ";
		}
		cout << " class: " << outputs[i][0] << endl << "--------------------------------------" << endl;

		elem[0] = abs( elem[0] - 0.2 );
		auto norm = ds.attr.get_normalizer();
		auto undo = norm.undo_normalize(input_attrs, elem);
		int undo_idx = 0;
		for (auto u_item : undo) {
			cout << input_attrs[undo_idx] << ": " << u_item.second << ", error: " << u_item.first << endl;
			undo_idx++;
		}
	}

}

int main () {
	/* xor example:*/
	//xor_example(100000);

	/*sine example:*/
	//sine_example(1000);
	
	load_dataset();

	return 0;
}
