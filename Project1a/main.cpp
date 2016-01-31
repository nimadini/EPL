#include <iostream>
#include "Vector.h"

using std::cout;
using std::endl;
using namespace epl;

int main(void) {
	cout << "Hello World!" << endl;

	vector<int> v;

	for (int i=0; i < 91; i++) {
		v.push_back(i);
	}

	v.print();

	return 0;
}