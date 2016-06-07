#include <iostream>
#include "Boosting.h"
#include "Dataset.h"
using namespace std;

void tennis_example( int iterations = 5 ) {
	//Data_set::_test_normalize_columns();

	string file_name = "../data/tennis.txt";
	string class_name = "Play";
	Data_set ds;
	ds.load_simple_db(file_name, class_name);

	Data_set train, test;
	ds.distribute_split(train, test, 0.6);

	Boosting boost{iterations};
	boost(train);
	boost.classify(test);
}

void zoo_example(int iterations = 5) {
	string file_name = "../data/zoo.txt";
	string class_name = "type";
	Data_set ds;
	ds.load_simple_db(file_name, class_name);

	Data_set train, test;
	ds.distribute_split(train, test, 0.8);

	Boosting boost{iterations};
	boost(train);
	boost.classify(test);
}

int main() {
	tennis_example();

	//zoo_example(); /*cannot work, the output is not binary!*/
	return 0;
}
