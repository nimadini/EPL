#pragma once
#ifndef _LinkedList_h
#define _LinkedList_h

#include <cstdint>
#include <iostream>

namespace epl {

template <typename T>
class Node {
public:
	Node(void) {
		std::cout << "in node default!\n";
	} 

	Node(T d, Node *n = NULL, Node *p = NULL) 
		: data(d), next(n), prev(p) {}

	~Node() {
		std::cout << "destructor being called.\n";
	}

	T data;
	Node *next;
	Node *prev;
};

template <typename T>
class list {
public:
	/*list(void) {
		head = last = nullptr;
	}*/

	~list(void) {
		std::cout << "destructor for list\n";
		destroy();
	}

	void push_back(Node<T> const & elem) {
		//Node<T> newNode{elem};
		Node<T>* newNode = (Node<T>*) ::operator new(sizeof(Node<T>));

		new (newNode) Node<T>{elem};

		if (!last) {
			std::cout << "last is nullptr\n";
			head = last = newNode;
			head->prev = nullptr;
			last->next = nullptr;
		} else {
			std::cout << "last is not nullptr\n";
			std::cout << "newNode: " << newNode->data << "\n";
			last->next = newNode;
			newNode->prev = last;
			newNode->next = nullptr;
			last = newNode;
		}
	}

	Node<T>& getLast() {
		return *last;
	}

private:
	Node<T>* head;
	Node<T>* last;

	void destroy() {
		Node<T>* cur = head;

		while (cur != last) {
			Node<T>* tmp = cur;
			cur = cur->next;
			delete tmp;
		}
	}

	void copy(list const& that) {

	}

	void my_move(list&& tmp) {

	}
};

} // Namespace epl.

#endif /* _LinkedList_h */
