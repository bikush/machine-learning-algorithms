#define _USE_MATH_DEFINES
#include <vector>
#include <algorithm>
#include <math.h>
#include "Knnalgorithm.h"
using namespace std;

double distance_func(const vector<double> & x1, const vector<double> & x2) {
	return  sqrt(pow(x2[0]-x1[0], 2) + pow((x2[1]-x1[1]), 2));
}

int main () {
    vector<vector<double>> inputs;
    vector<vector<double>> outputs;
    /*example: y = x1 + x2 */
    vector<double> x1;
    vector<double> x2;
    vector<double> y;
    int num_samples = 100;
    for (int i=0; i<num_samples; i++) {
    	for (int j=0; j<num_samples; j++) {
			x1.push_back(i*0.2);
			x2.push_back(j*0.2);
			y.push_back(i*0.2 * j*0.2);
    	}
    }
    inputs.push_back(x1);
    inputs.push_back(x2);
    outputs.push_back(y);
    Knn_algorithm knn{inputs, outputs, 5, &distance_func};
    vector<double>  res = knn(vector<double>{2.1, 3.2});
    cout << "res should be " << 2.1*3.2 <<", alg returned: " << res[0];
	return 0;
}

