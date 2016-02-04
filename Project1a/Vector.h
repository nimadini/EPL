// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _Vector_h
#define _Vector_h

#include <cstdint>
#include <iostream>

#define INITIAL_UNIT 16

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
		capacity = INITIAL_UNIT;
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
			new (&data[inc_mod(fidx+i)]) T{that.data[inc_mod(fidx+i)]};
		}
	}

	void destroy(void) {
		if (data) {
			int cnt = 0;
			for (uint64_t i = 0; i < size(); i++) {
				cnt++;
				data[inc_mod(fidx+i)].~T();
			}

			::operator delete(data); // deallocate memory (no destructors) 
		}
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
		return (i + 1) % capacity;
	}

	uint64_t dec_mod(uint64_t i) const {
		if (!i) {
			return capacity-1;
		}
		return (i - 1) % capacity;
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
			data = new T [size];
			fidx = eidx = 0;
			capacity = size;
			unit = INITIAL_UNIT;
		}
	}

	// Copy constructor.
	vector(vector const& that) {
		copy(that);
	}

	vector<T>& operator=(vector const& rhs) {
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

		return data[inc_mod(fidx+k)];
	}

	const T& operator[](uint64_t k) const {
		if (k < 0 || k >= size()) {
			throw std::out_of_range("subscript out of range");
		}

		return data[inc_mod(fidx+k)];
	}

	void push_back(const T& elem) {
		allocIfNull();

		// If there is no empty spot.
		if (!empty_spots()) {
			T* new_data = (T*) ::operator new((capacity+unit) * sizeof(T));

			for (uint64_t i = 0; i < size(); i++) {
				new (&new_data[i+1]) T{data[inc_mod(fidx+i)]};
			}

			fidx=0;
			eidx=capacity-1;
			capacity += unit;
			unit *= 2;

			destroy();
			this->data=new_data;
		}

		eidx = inc_mod(eidx);

		new (&this->data[eidx]) T{elem};
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

			for (uint64_t i = 0; i < size(); i++) {
				new (&new_data[i+1]) T{data[inc_mod(fidx+i)]}; // ????
			}

			fidx=0;
			eidx=capacity-1;
			capacity += unit;
			unit *= 2;

			destroy();
			this->data=new_data;
		}

		new (&this->data[fidx]) T{elem};

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
