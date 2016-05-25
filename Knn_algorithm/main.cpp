#define _USE_MATH_DEFINES
#include <vector>
#include <algorithm>
#include <math.h>
#include "Knnalgorithm.h"
using namespace std;

double distance_func(const vector<double> & x1, const vector<double> & x2) {
	return  sqrt(pow(x2[0]-x1[0], 2) + pow((x2[1]-x1[1]), 2));
}

double db_distance_func(const vector<double> & x1, const vector<double> & x2) {
	assert(x1.size() == x2.size());
	double count = 0.0;
	for (size_t i=0; i < x1.size(); i++) {
		if (abs(x1[i]-x2[i]) <= Knn_algorithm::tolerance) count +=1.0;
	}
	return count > 0 ? 1./count: 10e10;
}

void db_test(const string & file_name, const string & class_name, int k) {
	Data_set ds;
	ds.load_simple_db(file_name, class_name);
	Data_set train, test;
	ds.distribute_split(train, test, 0.8);
    Knn_algorithm knn{train, k, &db_distance_func, true};
    knn.test(test);
}

void zoo_example(int k=2) {
	//Data_set::_test_normalize_columns();
	cout << "ZOO example" << endl;
	string file_name = "../data/zoo.txt";
	string class_name = "type";
	db_test(file_name, class_name, k);
	cout << endl;
}

void tennis_example( int k=2) {
	//Data_set::_test_normalize_columns();
	cout << "Tennis example" << endl;
	string file_name = "../data/tennis.txt";
	string class_name = "Play";
	db_test(file_name, class_name, k);
	cout << endl;
}

void simple_example(int k=2) {
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
	double error = 0.;
	double max_error = 0.;
	double max_relative_error = 0.;
	int total = 0;
	int test_samples = 10;
	for (int i=0; i<test_samples; i++) {
		for (int j=0; j<test_samples; j++) {
			vector<double>  res = knn(vector<double>{i*0.1, j*0.1});
			total++;
			double real_res = i*i*0.1*0.1 + j*0.1;
			double err = abs(res[0]-real_res);
			error += err;
			max_error = max(max_error, err);
			if (abs(real_res) > 0.)
				max_relative_error = max(max_relative_error, abs(max_error/real_res));
		}
	}
	cout << "Simple example: y = x1^2 + x2." << endl;
	cout << "Average error: " << error/total << endl;
	cout << "Max error: " << max_error << endl;
	cout << "Max relative error: " << max_relative_error << endl << endl;
}
int main () {
	simple_example(1);
    zoo_example(3);
    tennis_example();
	return 0;
}

