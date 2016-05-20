
#include <iostream>
#include "Dataset.h"
#include "id3_algorithm.h"

using namespace std;


int main() {

/*	string file_name = "data/tennis.txt";
	string class_name = "Play";  // type
	string name = "Day";/**/

	string file_name = "../data/zoo.txt";
	string class_name = "type";  // type
	string name = "object";
/**/

	Data_set ds;
	ds.load_simple_db(file_name, class_name);
	
	Data_set train;
	Data_set test;
	ds.distribute_split(train, test, 0.65);

	cout << "data length: " << ds.get_size() << endl;
	id3_algorithm id3{ train };
	cout << "train data length: " << train.get_size() << endl;
	id3_node root = id3();

	root.print();
	
	int count = 0;
	for (int i=0; i< test.get_size(); i++) {
		Data d{test.get_elem(i)};
		string res = root.classify(d, class_name);
		if (d.get_value(class_name) == res){
			cout << d.get_value(name) << " is a " << res << endl;
			count++;
		}
		else
			cout << d.get_value(name) << " is not a " << res << endl;
	}
	cout << "Correctness: " << count/(double)test.get_size() << endl;
	return 0;
}
