
#include <iostream>
#include "Dataset.h"
#include "id3_algorithm.h"
#include "Cross_validation.h"

using namespace std;

void id3_test(string file_name, string class_name, string name) {
	Data_set ds;
	ds.load_simple_db(file_name, class_name);

	Data_set train;
	Data_set test;
	ds.distribute_split(train, test, 0.65, true);

	cout << "data length: " << ds.get_size() << endl;
	id3_algorithm id3{ train };
	cout << "train data length: " << train.get_size() << endl;
	id3_node root = id3();

	root.print();

	int count = 0;
	for (int i = 0; i< test.get_size(); i++) {
		Data d{ test.get_elem(i) };
		string res = root.classify(d, class_name);
		if (d.get_value(class_name) == res) {
			cout << d.get_value(name) << " is a " << res << endl;
			count++;
		}
		else
			cout << d.get_value(name) << " is not a " << res << endl;
	}
	cout << "Correctness: " << count / (double)test.get_size() << endl;
}

int main() {
	Cross_validation::test();
	

	//id3_test("../data/tennis.txt", "Play", "Day");
	//id3_test("../data/zoo.txt", "type", "object");
	
	return 0;
}
