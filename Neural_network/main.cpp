#include <iostream>
#include "Neuralnetwork.h"
using namespace std;

int main () {
	vector<int> init{2, 3, 1};
	vector<vector<double>> inputs(2);
	vector<vector<double>> outputs(1);
	inputs[0] = vector<double>{ 0, 0, 1, 1};
	inputs[1] = vector<double>{ 0, 1, 0, 1};
	/*xor*/
	outputs[0] = vector<double>{0, 0, 0, 1};
	Neural_network nn{init};
	nn(inputs, outputs, 100000);
	cout << " xor of 1, 1 is:" << nn.calculate(vector<double>{1.,1.})[0] << endl;
	cout << " xor of 1, 0 is:" << nn.calculate(vector<double>{1.,0.})[0] << endl;
	cout << " xor of 0, 1 is:" << nn.calculate(vector<double>{0.,1.})[0] << endl;
	cout << " xor of 0, 0 is:" << nn.calculate(vector<double>{0.,0.})[0] << endl;
	return 0;
}
