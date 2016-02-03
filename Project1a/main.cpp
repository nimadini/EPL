#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include "Vector.h"

using std::cout;
using std::endl;
using namespace epl;

#define UNIT 8
#define LARGE 50000

class exception {
	public:
		exception(std::string msg) { this->msg = msg; }
		std::string wtf(void) const { return msg; }
	private:
		std::string msg;
};

void test0(void) {
	vector<int> v;
	std::vector<int> std_v;

	for (int i=0; i < UNIT; i++) {
		v.push_back(i);
		std_v.push_back(i);

		if (v[i] != std_v[i]) {
			throw exception("test0 failed on []!");
		}
	}

	if (v.size() != std_v.size()) {
		throw exception("test0 failed on size!");
	}
}

void test0_extreme(void) {
	vector<int> v;
	std::vector<int> std_v;

  	srand (time(NULL));

	for (int i=0; i < LARGE; i++) {
		int num=rand() % 10;
		v.push_back(num);
		std_v.push_back(num);

		if (v[i] != std_v[i]) {
			throw exception("test0_extreme failed on []!");
		}
	}

	if (v.size() != std_v.size()) {
		throw exception("test0_extreme failed on size!");
	}
}

void test1(void) {
	vector<int> v;
	std::vector<int> std_v;

	for (int i=0; i < UNIT; i++) {
		v.push_front(i);
		std_v.insert(std_v.begin(), i);

		if (v[i] != std_v[i]) {
			throw exception("test1 failed on []!");
		}
	}

	for (int i=0; i < UNIT; i++) {
		if (v[i] != std_v[i]) {
			throw exception("test1 failed on []!");
		}
	}

	if (v.size() != std_v.size()) {
		throw exception("test1 failed on size!");
	}
}

void test1_extreme(void) {
	vector<int> v;
	std::vector<int> std_v;

  	srand (time(NULL));

	for (int i=0; i < LARGE; i++) {
		int num=rand() % 10;

		v.push_front(num);
		std_v.insert(std_v.begin(), num);

		if (v[i] != std_v[i]) {
			throw exception("test1_extreme failed on []!");
		}

		if (v.size() != std_v.size()) {
			throw exception("test1_extreme failed on size 1!");
		}
	}

	for (int i=0; i < LARGE; i++) {
		if (v[i] != std_v[i]) {
			throw exception("test1_extreme failed on []!");
		}
	}

	if (v.size() != std_v.size()) {
		throw exception("test1_extreme failed on size 2!");
	}
}

void test2(void) {
	vector<int> v;
	std::vector<int> std_v;

	for (int i=0; i < UNIT; i++) {
		v.push_front(i);
		v.push_back(i);

		std_v.insert(std_v.begin(), i);
		std_v.push_back(i);

		if (v[i] != std_v[i]) {
			throw exception("test2 failed on []!");
		}
	}

	for (int i=0; i < UNIT; i++) {
		if (v[i] != std_v[i]) {
			throw exception("test2 failed on []!");
		}
	}

	if (v.size() != std_v.size()) {
		throw exception("test2 failed on size!");
	}
}

void test2_extreme(void) {
	vector<int> v;
	std::vector<int> std_v;

	srand (time(NULL));

	for (int i=0; i < LARGE; i++) {
		int num=rand() % 10;

		v.push_front(num);
		std_v.insert(std_v.begin(), num);

		num=rand() % 10;

		std_v.push_back(num);
		v.push_back(num);

		if (v[i] != std_v[i]) {
			throw exception("test2 failed!");
		}
	}

	for (int i=0; i < LARGE; i++) {
		if (v[i] != std_v[i]) {
			throw exception("test2 failed!");
		}
	}
}

void test3(void) {
	vector<int> v;

	for (int i=0; i < LARGE; i++) {
		v.push_back(i);

		if (v.size() != i+1) {
			throw exception("test3 failed on size 1!");
		}
	}

	for (int i=0; i < LARGE; i++) {
		v.pop_front();
	}

	if (v.size() != 0) {
		throw exception("test3 failed on size 2!");
	}
}

void test4(void) {
	vector<int> v(4);

	for (int i=0; i < UNIT; i++) {
		v.push_back(i);

		if (v.size() != i+1) {
			throw exception("test4 failed on size 1!");
		}
	}

	for (int i=0; i < UNIT; i++) {
		v.pop_front();
	}

	if (v.size() != 0) {
		throw exception("test4 failed on size 2!");
	}
}

void foo(vector<int> v, std::vector<int> std_v, std::string msg) {

	for (int i=0; i < UNIT; i++) {
		if (v[i] != std_v[i]) {
			throw exception(msg);
		}
	}
}

void test5(void) {
	vector<int> v;
	std::vector<int> std_v;

	for (int i=0; i < UNIT; i++) {
		v.push_back(i);
		std_v.push_back(i);
	}

	foo(v, std_v, "test5 failed in foo 1!");

	for (int i=0; i < UNIT; i++) {
		v.push_front(i);
		std_v.insert(std_v.begin(), i);
	}

	foo(v, std_v, "test5 failed in foo 2!");

	for (int i=0; i < UNIT/5; i++) {
		v.pop_front();
		std_v.erase(std_v.begin());
	}

	foo(v, std_v, "test5 failed in foo 3!");

	for (int i=0; i < UNIT/5; i++) {
		v.pop_back();
		std_v.erase(std_v.begin() + std_v.size()-1);
	}

	foo(v, std_v, "test5 failed in foo 4!");
}

void test6(void) {
	vector<int> v;
	std::vector<int> std_v;

	for (int i=0; i < UNIT; i++) {
		v.push_back(i);
		std_v.push_back(i);
	}

	vector<int> v2 = v;

	foo(v2, std_v, "test6 failed in foo 1!");

	for (int i=0; i < UNIT; i++) {
		v2.push_front(i+1);
		std_v.insert(std_v.begin(), i+1);
	}

	vector<int> v3 = v2;

	foo(v3, std_v, "test6 failed in foo 2!");

	for (int i=0; i < UNIT/5; i++) {
		v3.pop_front();
		std_v.erase(std_v.begin());
	}

	foo(v3, std_v, "test6 failed in foo 3!");

	for (int i=0; i < UNIT/5; i++) {
		v3.pop_back();
		std_v.erase(std_v.begin() + std_v.size()-1);
	}

	vector<int> v4 = v3;

	foo(v4, std_v, "test6 failed in foo 4!");
}

int main(void) {
	try {
		test0();
	} catch(exception e) {
		cout << e.wtf() << std::endl;
	}

	try {
		test0_extreme();
	} catch(exception e) {
		cout << e.wtf() << std::endl;
	}

	try {
		test1();
	} catch(exception e) {
		cout << e.wtf() << std::endl;
	}

	try {
		test1_extreme();
	} catch(exception e) {
		cout << e.wtf() << std::endl;
	}

	try {
		test2();
	} catch(exception e) {
		cout << e.wtf() << std::endl;
	}

	try {
		test2_extreme();
	} catch(exception e) {
		cout << e.wtf() << std::endl;
	}

	try {
		test3();
	} catch(exception e) {
		cout << e.wtf() << std::endl;
	}

	try {
		test4();
	} catch(exception e) {
		cout << e.wtf() << std::endl;
	}

	try {
		test5();
	} catch(exception e) {
		cout << e.wtf() << std::endl;
	}

	try {
		test6();
	} catch(exception e) {
		cout << e.wtf() << std::endl;
	}

	return 0;
}