#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include "Dataset.h"
#include "id3_algorithm.h"

using namespace std;

istream & operator>>(istream & is, Data & d){
	for (const auto & attr: d.attributes) {
		string val;
		if (is >> val) {
			d.set_value(attr, val);
		}else{
			throw runtime_error("Invalid data format");
		}
	}
	return is;
}

// trim from start
static inline string &ltrim(string &s) {
        s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
        return s;
}

// trim from end
static inline string &rtrim(string &s) {
        s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline string &trim(string &s) {
        return ltrim(rtrim(s));
}

int main() {
/*
	ifstream ifs{"data/tennis.txt"};
	string class_name = "Play";  // type
	string name = "Day";/**/

	ifstream ifs{"data/zoo.txt"};
	string class_name = "type";  // type
	string name = "object";
/**/
	if (!ifs) throw runtime_error("Input file not found.");
	string line;
	bool db_flag = true;
	bool metadata = false;
	string db_name;
	Data_set ds{class_name};
	while (getline(ifs, line)) {
		line = trim(line);
		if (line!= "" && line[0] != ';') {
			if (db_flag) {
				db_flag = false;
				metadata = true;
				db_name = line;
				// the database name is the first line
			} else {
				istringstream is{line};
				if (metadata) {
					metadata = false;
					string attr_val;
					string attr_type;
					while (is>>attr_val && is>>attr_type) {
						Attribute attr{attr_val, attr_type, !(attr_type == "name" || attr_val == class_name)};
						ds.attr.append_attr(attr);
					}
				} else {
					Data d{ds.attr.get_all_attributes()};
					is>>d;
					ds.append_data(d);
				}
			}

		}

	}

	/***********************finished with parsing of the input data*************************************/


	
/*
	cout << "data length: " << ds.get_size() << endl;
	id3_algorithm id3{ds, 0.8};
	cout << "train data length: " << ds.get_train_set_size() << endl;
	id3_node root = id3();

	int count = 0;
	for (int i = 0; i< ds.get_test_set_size(); i++) {
		Data d{ ds.get_test_elem(i) };
		string res = root.classify(d, class_name);
		if (d.get_value(class_name) == res) {
			cout << d.get_value(name) << " is a " << res << endl;
			count++;
		}
		else
			cout << d.get_value(name) << " is not a " << res << endl;
	}
	cout << "Correctness: " << count / (double)ds.get_test_set_size() << endl;
	return 0;
	*/
	
	Data_set train;
	Data_set test;
	ds.distribute_split(train, test, 0.5);

	cout << "data length: " << train.get_size() << endl;
	id3_algorithm id3{ train, true };
	cout << "train data length: " << train.get_train_set_size() << endl;
	id3_node root = id3();

	root.print();
	
	int count = 0;
	for (int i=0; i< test.get_test_set_size(); i++) {
		Data d{test.get_test_elem(i)};
		string res = root.classify(d, class_name);
		if (d.get_value(class_name) == res){
			cout << d.get_value(name) << " is a " << res << endl;
			count++;
		}
		else
			cout << d.get_value(name) << " is not a " << res << endl;
	}
	cout << "Correctness: " << count/(double)test.get_test_set_size() << endl;
	return 0;
}
