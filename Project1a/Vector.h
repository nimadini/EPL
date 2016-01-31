// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _Vector_h
#define _Vector_h

#include <cstdint>
#include <iostream>

using std::cout;

namespace epl {

const int default_size = 8;

template <typename T>
class vector {
private:
	T* data;
	uint64_t begin;
	uint64_t end;
	uint64_t capacity;

	void init(void) {
		this->data=nullptr;
		this->begin=end=0;
		this->capacity=default_size;
	}

	void copy(vector const& that) {
		this->begin=that.begin;
		this->end=that.end;
		this->capacity=that.capacity;
		this->data=new T[capacity];

		for (uint64_t i=0; i<capacity; i++) {
			this->data[i]=that.data[i];
		}
	}

	void destroy(void) {
		// Field data might be nullptr.
		if (this->data) {
			delete[] this->data;
		}
	}

public:
	// Default constructor.
	vector(void) {
		init();
	}

	// Destructor.
	~vector(void) {
		destroy();
	}

	// Constructor.
	vector(uint64_t size) {
		if (!size) {
			init();
		} else {
			this->data=new T[size];
			this->capacity=size;
			this->begin=end=0;
		}
	}

	// Copy constructor.
	vector(vector const& that) {
		copy(that);
	}

	vector& operator=(vector const& rhs) {
		if (this != &rhs) {
			destroy();
			copty(rhs);
		}

		return *this;
	}

	uint64_t size(void) const {
		return end-begin;
	}

	T& operator[](uint64_t k) {
		if (k >= size()) {
			throw std::out_of_range("subscript out of range");
		}

		return data[begin+k+1];
	}

	const T& operator[](uint64_t k) const {
		if (k >= size()) {
			throw std::out_of_range("subscript out of range");			
		}

		return data[begin+k+1];
	}

	void push_back(const T& elem) {
		// Check if data has not yet been initialized (nullptr).
		if (!this->data) {
			this->data=new T[capacity];
		}

		// Check if data[] is large enough to fit the new element.
		if (end+1 == capacity) {
			capacity*=2; // Double the capacity.

			T* newData=new T[capacity];

			for (uint64_t i=begin; i < end; i++) {
				newData[i+1]=this->data[i+1];
			}

			destroy();
			this->data=newData;
		}

		T newElem{elem}; // Calling T's copy constructor.
		this->data[++end]=newElem;
	}

	void push_front(const T&) {
		if (!begin) {

		}
	}

	void print(void) {
		for (uint64_t i=begin; i<end; i++) {
			cout << data[i+1] << " ";
		}
	}
};

} // Namespace epl.

#endif /* _Vector_h */
