#pragma once

namespace epl {

class B {
public:
	B(void) {
		// cout << "B default constructor.\n";
	}

	~B(void) {
		// cout << "B default destructor.\n";
	}

	B(B const& that) {
		// cout << "B copy constructor.\n";
	}

	B& operator=(B const& rhs) {
		throw std::domain_error("You should not call the assignment operator of T in this assignment!!!");         
	}
};

}