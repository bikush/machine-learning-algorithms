#include <iostream>
#include "Neuralnetwork.h"
using namespace std;

int main () {
	vector<int> init{2, 3, 1};
	vector<vector<double>> inputs(2);
	vector<vector<double>> outputs(1);
	inputs[0] = vector<double>{ 0.1, 0.1, 1, 1};
	inputs[1] = vector<double>{ 0.1, 1, 0.1, 1};
	/*xor*/
	outputs[0] = vector<double>{0.1, 0.1, 0.1, 1};
	Neural_network nn{init, 0.5};
	nn(inputs, outputs, 1000000);
	cout << " or of 1, 1 is 1: " << nn.calculate(vector<double>{1, 1})[0] << endl;
	cout << " or of 1, 0.1 is 0.1: " << nn.calculate(vector<double>{1., 0.1})[0] << endl;
	cout << " or of 0.1, 1 is 0.1: " << nn.calculate(vector<double>{0.1, 1.})[0] << endl;
	cout << " or of 0.1, 0.1 is 0.1: " << nn.calculate(vector<double>{0.1, 0.1})[0] << endl;
	return 0;
}
