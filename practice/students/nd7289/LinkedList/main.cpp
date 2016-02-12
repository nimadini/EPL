#include <iostream>
#include <vector>
#include <string>
#include "LinkedList.h"

using std::cerr;
using std::endl;
using std::string;
using namespace epl;

int main(void) {
	// list<int> l;

	Node<int> n{10};
	list<int> l;

	std::cout << "PUSHING BACK\n";
	l.push_back(n);

	std::cout << l.getLast().data << "\n";

	std::cout << std::boolalpha;

    // std::cout << "node: " << std::is_pod<Node<int>>::value << '\n';
    // std::cout << "list: " << std::is_pod<list<int>>::value << '\n';

	return 0;
}