#include <iostream>
#include "Neuralnetwork.h"
using namespace std;

int main () {
	/* xor example:*/
	/*
	vector<int> init{2, 3, 1};
	vector<vector<double>> inputs(2);
	vector<vector<double>> outputs(1);
	inputs[0] = vector<double>{ 0, 0, 1, 1};
	inputs[1] = vector<double>{ 0, 1, 0, 1};
	outputs[0] = vector<double>{0, 1, 1, 0};
	Neural_network nn{init, 0.1};
	nn(inputs, outputs, 10000);

	Neural_network::test(nn, vector<double>{1, 1}, vector<double>{0});
	Neural_network::test(nn, vector<double>{1., 0}, vector<double>{1});
	Neural_network::test(nn, vector<double>{0, 1.}, vector<double>{1});
	Neural_network::test(nn, vector<double>{0, 0}, vector<double>{0});
	/**/

	/*sine example:*/

	vector<int> init{1, 2, 1};
	vector<vector<double>> inputs(1);
	vector<vector<double>> outputs(1);
	Neural_network nn{init, 0.1};

	inputs[0] = vector<double>(50);
	outputs[0] = vector<double>(50);
	for (size_t i=0; i < inputs[0].size(); i++) {
		inputs[0][i] = i*M_PI*2./double(inputs[0].size());
		outputs[0][i] = sin(inputs[0][i]);
	}
	nn(inputs, outputs, 10000);

	Neural_network::test(nn, vector<double>{0.}, vector<double>{sin(0.)});
	Neural_network::test(nn, vector<double>{0.5}, vector<double>{sin(0.5)});
	Neural_network::test(nn, vector<double>{1.5}, vector<double>{sin(1.5)});
	Neural_network::test(nn, vector<double>{3.1}, vector<double>{sin(3.1)});
	Neural_network::test(nn, vector<double>{4.}, vector<double>{sin(4.)});
	Neural_network::test(nn, vector<double>{4.5}, vector<double>{sin(4.5)});
	/**/

	return 0;
}
