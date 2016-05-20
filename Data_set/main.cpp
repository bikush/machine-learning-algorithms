
#include <iostream>
#include "../Dataset.h"

using namespace std;


int main() {

	/*	string file_name = "data/tennis.txt";
	string class_name = "Play";  // type
	string name = "Day";/**/

	string file_name = "data/zoo.txt";
	string class_name = "type";  // type
	string name = "object";
	/**/

	Data_set ds;
	ds.load_simple_db(file_name, class_name);

	Data_set train;
	Data_set test;
	ds.distribute_split(train, test, 0.65);

	cout << "data length: " << ds.get_size() << endl;
	cout << "train data length: " << train.get_size() << endl;
	
	return 0;
}
