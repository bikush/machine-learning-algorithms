
#include "Neuralnetwork.h"
#include "Knnalgorithm.h"
#include "id3_algorithm.h"
#include "Boosting.h"
#include "Cross_validation.h"

int main () {
	Neural_network::run_examples();
	//Knn_algorithm::run_examples();
	//id3_algorithm::run_examples();
	Boosting::run_examples();
	//Cross_validation::run_examples();

	return 0;
}
