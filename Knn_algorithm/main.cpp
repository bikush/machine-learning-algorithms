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
	int num_samples = 100;
    vector<vector<double>> inputs(num_samples*num_samples, vector<double>(2));
    vector<vector<double>> outputs(num_samples*num_samples, vector<double>(1));
    /*example: y = x1^2 + x2*/
    for (int i=0; i<num_samples; i++) {
    	for (int j=0; j < num_samples; j++) {
    		inputs[i*num_samples+j][0] = i*0.2;
			inputs[i*num_samples+j][1] = j*0.2;
			outputs[i*num_samples+j][0] = i*i*0.2*0.2 + j*0.2;
    	}
    }
    Knn_algorithm knn{inputs, outputs, 1, &distance_func};
    vector<double>  res = knn(vector<double>{2.1, 3.1});
    cout << "res should be " << 2.1*2.1+3.1 <<", alg returned: " << res[0];
	return 0;
}

