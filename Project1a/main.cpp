#include <iostream>
#include <vector>
#include <string>
#include "Vector.h"
#include "B.h"

using std::cerr;
using std::endl;
using std::string;
using namespace epl;

#define UNIT 8
#define MEDIUM 100
#define LARGE 5000

class exception {
	public:
		exception(std::string msg) { this->msg = msg; }
		std::string wtf(void) const { return msg; }
	private:
		std::string msg;
};

/**
 Testing push_back for values & size()
*/
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

/**
 Tests push_back for values & size() for HUGE # of elements
*/
void test0_extreme(void) {
	vector<int> v;
	std::vector<int> std_v;

	for (int i=0; i < LARGE; i++) {
		v.push_back(i);
		std_v.push_back(i);

		if (v[i] != std_v[i]) {
			throw exception("test0_extreme failed on []!");
		}
	}

	if (v.size() != std_v.size()) {
		throw exception("test0_extreme failed on size!");
	}
}

/**
 Tests push_front for values & size()
*/
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

/**
Tests push_front for values & size()
*/
void test1_extreme(void) {
	vector<int> v;
	std::vector<int> std_v;

	for (int i=0; i < LARGE; i++) {
		v.push_front(i);
		std_v.insert(std_v.begin(), i);

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

/**
 Tests alternating push_front & push_back for values & size()
*/
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

/**
 Tests alternating push_front & push_back for values & size() for HUGE sizes
*/
void test2_extreme(void) {
	vector<int> v;
	std::vector<int> std_v;

	for (int i=0; i < LARGE; i++) {
		v.push_front(i);
		std_v.insert(std_v.begin(), i);

		std_v.push_back(i);
		v.push_back(i);

		if (v[i] != std_v[i]) {
			throw exception("test2_extreme failed on []!");
		}
	}

	for (int i=0; i < LARGE; i++) {
		if (v[i] != std_v[i]) {
			throw exception("test2_extreme failed on size!");
		}
	}
}

/**
 Test growing size w/ push_back and pop_front to get to size 0
*/
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

/**
 Test growing size w/ push_front and pop_back to get to size 0
*/
void test4(void) {
	vector<int> v;

	for (int i=0; i < LARGE; i++) {
		v.push_front(i);

		if (v.size() != i+1) {
			throw exception("test4 failed on size 1!");
		}
	}

	for (int i=0; i < LARGE; i++) {
		v.pop_back();
	}

	if (v.size() != 0) {
		throw exception("test4 failed on size 2!");
	}
}

void foo(vector<int> v, std::vector<int> std_v, std::string msg) {

	for (int i=0; i < LARGE; i++) {
		if (v[i] != std_v[i]) {
			throw exception(msg);
		}
	}
}

/**
 Tests push & pop for front & back looking at values
*/
void test5(void) {
	vector<int> v;
	std::vector<int> std_v;

	for (int i=0; i < LARGE; i++) {
		v.push_back(i);
		std_v.push_back(i);
	}

	foo(v, std_v, "test5 failed in foo 1!");

	for (int i=0; i < LARGE; i++) {
		v.push_front(i);
		std_v.insert(std_v.begin(), i);
	}

	foo(v, std_v, "test5 failed in foo 2!");

	for (int i=0; i < LARGE/5; i++) {
		v.pop_front();
		std_v.erase(std_v.begin());
	}

	foo(v, std_v, "test5 failed in foo 3!");

	for (int i=0; i < LARGE/5; i++) {
		v.pop_back();
		std_v.erase(std_v.begin() + std_v.size()-1);
	}

	foo(v, std_v, "test5 failed in foo 4!");
}

/**
 Tests assignment operator
*/
void test6(void) {
	vector<int> v;
	std::vector<int> std_v;

	for (int i=0; i < LARGE; i++) {
		v.push_back(i);
		std_v.push_back(i);
	}

	vector<int> v2 = v;

	foo(v2, std_v, "test6 failed in foo 1!");

	for (int i=0; i < LARGE; i++) {
		v2.push_front(i+1);
		std_v.insert(std_v.begin(), i+1);
	}

	vector<int> v3 = v2;

	foo(v3, std_v, "test6 failed in foo 2!");

	for (int i=0; i < LARGE/5; i++) {
		v3.pop_front();
		std_v.erase(std_v.begin());
	}

	foo(v3, std_v, "test6 failed in foo 3!");

	for (int i=0; i < LARGE/5; i++) {
		v3.pop_back();
		std_v.erase(std_v.begin() + std_v.size()-1);
	}

	vector<int> v4 = v3;

	foo(v4, std_v, "test6 failed in foo 4!");
}

/**
 Tests vector with non-pod, string
*/
void test7(void) {
	vector<std::string> v;
	std::vector<std::string> std_v;

	std::string base="c";

	for (int i = 0; i < LARGE; i++) {
		std::string cur=base + std::to_string(i);
		v.push_back(cur);
		std_v.push_back(cur);
	}

	for (int i = 0; i < LARGE; i++) {
		if (v[i] != std_v[i]) {
			throw exception("test7 failed on [] 1!");
		}
	}
}

/**
 Tests vector<vector<int>>
*/
void test8_A(void) {
	vector<vector<int>> super;
	std::vector<std::vector<int>> std_super;

	for (int i = 0; i < LARGE; i++) {
		vector<int> sub;
		std::vector<int> std_sub;

		for (int j = 0; j < MEDIUM; j++) {
			sub.push_back(j);
			std_sub.push_back(j);

			if (sub[j] != std_sub[j]) {
				throw exception("test8 failed on [] 1!");
			}
		}

		super.push_back(sub);
		std_super.push_back(std_sub);
	}

	for (int i = 0; i < LARGE; i++) {
		for (int j = 0; j < MEDIUM; j++) {
			if (super[i][j] != std_super[i][j]) {
				throw exception("test8 failed on [] 2!");
			}
		}
	}

	if (super.size() != std_super.size()) {
		throw exception("test8 failed on size!");
	}
}

/**
 Tests vector<vector<int>> assignment operator
*/
void test8_B(void) {
	vector<vector<int>> super;
	std::vector<std::vector<int>> std_super;

	for (int i = 0; i < LARGE; i++) {
		vector<int> sub;
		std::vector<int> std_sub;

		for (int j = 0; j < MEDIUM; j++) {
			sub.push_back(j);
			std_sub.push_back(j);

			if (sub[j] != std_sub[j]) {
				throw exception("test8 failed on [] 1!");
			}
		}

		super.push_back(sub);
		std_super.push_back(std_sub);
	}

	vector<vector<int>> super2 = super;

	for (int i = 0; i < LARGE; i++) {
		for (int j = 0; j < MEDIUM; j++) {
			if (super2[i][j] != std_super[i][j]) {
				throw exception("test8 failed on [] 2!");
			}
		}
	}

	if (super2.size() != std_super.size()) {
		throw exception("test8 failed on size!");
	}
}

/**
 Tests something non-POD?
*/
void test9(void) {
	vector<B> v;

	B b;
	// cerr << "{\n";
	v.push_back(b);
	// cerr << "}\n";
}

/**
 Tests supplied capacity w/ size()
*/
void test10_A(void) {
	vector<int> v(1);

	v.push_back(0);
	v.pop_front();

	if (v.size() != 1) {
		throw exception("test10_A failed on size!");
	}
}

/**
 Tests supplied capacity w/ size() large
*/
void test10_B(void) {
	vector<int> v(1);

	for (int i = 0; i < LARGE; i++) {
		v.push_back(i);
	}

	if (v.size() != LARGE+1) {
		throw exception("test10_B failed on size 1!");
	}

	v.pop_front();
	v.pop_back();

	if (v.size() != LARGE-1) {
		throw exception("test10_B failed on size 2!");
	}
}

/**
Tests supplied capacity w/ size() grow & shrink to nothing
*/
void test10_C(void) {
	vector<int> v(UNIT);

	for (int i = 0; i < UNIT; i++) {
		v.push_front(i);
	}

	if (v.size() != UNIT*2) {
		throw exception("test10_C failed on size 1!");
	}

	for (int i = 0; i < UNIT; i++) {
		v.pop_back();
		v.pop_front();
	}

	if (v.size() != 0) {
		throw exception("test10_C failed on size 2!");
	}
}

/**
 Tests supplied constructor w/ default values
*/
void test11(void) {
	vector<int> v(10);

	for (int i=0; i < 10; i++) {
		if (v[i] != 0) {
			throw exception("test11 failed on []!");
		}
	}
}

/**
 Test supplied constructor w/ assignment operator for size
*/
void test12_A(void) {
	vector<int> v(4);
	vector<int> v2 = v;

	if (v.size() != 4 || v.size() != v2.size()) {
		throw exception("test11 failed on size stuff!");
	}
}

/**
 Tests out of range exception
*/
void test12_B(void) {
	vector<int> v;
	v[0];
}

/**
 Tests deep copy assignment & constructor
*/
void test12_C(void) {
	vector<string> v1{4};
	v1[0] = "A";
	v1[1] = "B";
	v1[2] = "C";
	v1[3] = "D";

	vector<string> v2 = v1;

	v2[0] = "Z";

	if (v1[0] == "Z")
	{
		throw exception("test12_C failed on copy assignment!");
	}

	vector<string> v3{ v1 };
	v3[0] = "Y";
	if (v1[0] == "Y")
	{
		throw exception("test12_C failed on copy constructor!");
	}
}

void test13(void)
{
	const vector<int> x{5};
	if (x[2] != 0)
	{
		throw exception("test13 failed on const []!");
	}
}

int main(void) {
	try {
		test0();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test0_extreme();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	} 

	try {
		test1();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test1_extreme();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test2();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test2_extreme();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test3();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test4();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test5();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test6();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}
	
	try {
		test7();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test8_A();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test8_B();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test9();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test10_A();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test10_B();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test10_C();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test11();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test12_A();
	} catch(exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test12_B();
		cerr << "failed in 12 :: not throwing an expected exception! (out_of_range)\n";
	} catch(std::out_of_range e) {}

	try {
		test12_C();
	}
	catch (exception e) {
		cerr << e.wtf() << std::endl;
	}

	try {
		test13();
	}
	catch (exception e) {
		cerr << e.wtf() << std::endl;
	}

	return 0;
}