#pragma once

namespace epl {

class B {
public:
	B(void) {
		cout << "B default constructor.\n";
	}

	~B(void) {
		cout << "B default destructor.\n";
	}

	B(int i) {
		cout << "B int constructor.\n";
	}

	B(B const& that) {
		cout << "B copy constructor.\n";
	}

	B& operator=(B const& rhs) {
		cout << "B copy assignment operator.\n";             
		return *this;
	}
};

}