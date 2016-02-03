// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _Vector_h
#define _Vector_h

#include <cstdint>
#include <iostream>

#define INITIAL_UNIT 8

using std::cout;

namespace epl {

template <typename T>
class vector {
private:
	T* data;
	uint64_t capacity;
	uint64_t fidx;
	uint64_t eidx;
	uint64_t unit;

	void init(void) {
		data = nullptr;
		capacity = INITIAL_UNIT; // ????
		fidx = eidx = 0;
		unit = INITIAL_UNIT;
	}

	void copy(vector const & that) {
		fidx = that.fidx;
		eidx = that.eidx;
		capacity = that.capacity;
		unit = that.unit; // TODO: think more...

		if (!that.data) {
			data = nullptr;
			return;
		}

		// allocate mem but don't initialize it. 
		data = (T*) ::operator new(capacity * sizeof(T));
		
		for (uint64_t i = 0; i < size(); i++) {
			data[(fidx+1+i) % capacity] = that.data[(fidx+1+i) % capacity];
		}
	}

	void destroy(void) {
		if (data) {
			for (uint64_t i = 0; i < size(); i++) {
				data[(fidx+1+i) % capacity].~T();
			}

			::operator delete(data); // deallocate memory (no destructors) 
		}
		// why can't we use delete[]?
	}

	void allocIfNull(void) {
		// Check if data has not been allocated (nullptr).
		if (!data) {
			data = (T*) ::operator new(capacity * sizeof(T));
		}
	}

	uint64_t empty_spots(void) const {
		return capacity - size() - 1; 
	}

	uint64_t inc_mod(uint64_t i) const {
		return modulo(i+1, capacity);
	}

	uint64_t dec_mod(uint64_t i) const {
		return modulo(i-1, capacity);
	}

	int modulo(int i, int n) const {
	  const int k = i % n;
	  return k < 0 ? k + n : k;
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
			data = new T [size]; // (T*) ::operator new(size * sizeof(T));
			fidx = eidx = 0;
			capacity = size;
			unit = INITIAL_UNIT;
		}
	}

	// Copy constructor.
	vector(vector const& that) {
		copy(that);
	}

	vector& operator=(vector const& rhs) {
		if (this != &rhs) {
			destroy();
			copy(rhs);
		}

		return *this;
	}

	uint64_t size(void) const {
		if (eidx >= fidx) {
			return eidx - fidx;
		}

		return capacity - (fidx - eidx);
	}

	T& operator[](uint64_t k) {
		if (k < 0 || k >= size()) {
			throw std::out_of_range("subscript out of range");
		}

		return data[(fidx + 1 + k) % capacity];
	}

	const T& operator[](uint64_t k) const {
		if (k < 0 || k >= size()) {
			throw std::out_of_range("subscript out of range");
		}

		return data[(fidx + 1 + k) % capacity];
	}

	void push_back(const T& elem) {
		allocIfNull();

		// If there is no empty spot.
		if (!empty_spots()) {
			T* new_data = (T*) ::operator new((capacity+unit) * sizeof(T));

			for (uint64_t i = 0; i < capacity; i++) {
				new_data[i] = data[(fidx + i)%capacity]; // ????
			}

			fidx=0;
			eidx=capacity-1;
			capacity += unit;
			unit *= 2;

			destroy();
			this->data=new_data;
		}

		//T newElem{elem}; // Calling T's copy constructor???

		eidx = inc_mod(eidx);
		this->data[eidx]=T{elem};
	}

	void pop_back(void) {
		// If array is empty.
		if (!size()) {
			throw std::out_of_range("subscript out of range");
		}

		this->data[eidx].~T();
		eidx = dec_mod(eidx);
	}

	void push_front(const T& elem) {
		allocIfNull();
		
		if (!empty_spots()) {
			T* new_data = (T*) ::operator new((capacity+unit) * sizeof(T));

			for (uint64_t i = 0; i < capacity; i++) {
				new_data[i] = data[(fidx + i)%capacity]; // ????
			}

			fidx=0;
			eidx=capacity-1;
			capacity += unit;
			unit *= 2;

			destroy();
			this->data=new_data;
		}

		this->data[fidx]=T{elem};
		fidx = dec_mod(fidx);
	}

	void pop_front(void) {
		// If array is empty.
		if (!size()) {
			throw std::out_of_range("subscript out of range");
		}

		fidx = inc_mod(fidx);
		this->data[fidx].~T();
	}

	void print(void) {
		for (uint64_t i = 0; i < size(); i++) {
			cout << data[(fidx+1+i) % capacity] << " ";
		}

		cout << "\n";
	}
};

} // Namespace epl.

#endif /* _Vector_h */
