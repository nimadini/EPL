#include <iostream>
#include <vector>

#include "Vector.h"

using std::cout;
using std::endl;
using namespace epl;

int main(void) {
	cout << "Hello World!" << endl;

	vector<int> v;

	std::vector<int> std_v;

	for (int i=0; i < 10; i++) {
		std_v.push_back(i);
		// v.print();
		std_v.insert(std_v.begin(), i);

		// v.push_back(i);
		v.push_back(i);
		v.push_front(i);
	//	v.push_front(i);
		// v.push_front(i);
	}

	cout << endl;

	// v.print();

	/*for (int i = 0; i < 4; i++) {
		v.pop_back();
	}*/

	v.print();

	for (int i=0; i<std_v.size(); i++) {
		cout << std_v[i] << " ";
	}

	// cout << v[0] << v[1] << v[2];

	// cout << "\n";

	return 0;
}