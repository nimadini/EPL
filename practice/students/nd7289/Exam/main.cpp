#include <iostream>
#include <vector>
#include <string>
#include "templ.h"

using std::cerr;
using std::endl;
using std::string;
using namespace epl;

int main(void) {
	Base* array = new Derived[10];
	array->doit();
	array->foo();

	std::cout << "base size: " << sizeof(Base) << "\n";
	std::cout << "derived size: " << sizeof(Derived) << "\n";

	std::cout << "array addr: " << array << "\n";
	std::cout << "array + base addr: " << &array[0] + sizeof(Base) << "\n";
	std::cout << "array + derived addr: " << &array[0] + sizeof(Derived) << "\n";
	(array + sizeof(Base)) ->doit();

	Derived* array_derived = (Derived*) array;

	std::cout << "array[0] addr: " << &array_derived[0] << "\n";
	std::cout << "array[1] addr: " << &array_derived[1] << "\n";


	array_derived[0].doit();
	array_derived[1].doit();

	tfun1(*array);
	tfun2(*array);

	return 0;
}