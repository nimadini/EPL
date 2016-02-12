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
	T* data; 			// contains the elements of the vector (it's a circular array)
						// if fidx == eidx: array is empty
						// else if (eidx + 1) % capacity == fidx: array is full
						// so we have one redundant spot in the array to distinguish between
						// the given 2 scenarios

	uint64_t capacity; 	// the capacity
	uint64_t fidx; 		// front index
	uint64_t eidx; 		// end index
	uint64_t unit;		// unit for dynamic doubling

	// init a vector: data would be null, fidx
	// and eidx both point to the 1st elem
	void init(void) {
		data = nullptr;
		capacity = INITIAL_UNIT;
		fidx = eidx = 0;
		unit = INITIAL_UNIT;
	}

	// copy constructor logic
	void copy(vector const & that) {
		fidx = that.fidx;
		eidx = that.eidx;
		capacity = that.capacity;
		unit = that.unit;

		// if "that" has no data
		if (!that.data) {
			data = nullptr;
			return;
		}

		// allocate mem but don't initialize it. 
		data = (T*) ::operator new(capacity * sizeof(T));
		
		// do the copy one-by-one by calling T{elem}, i.e., copy constructor of T
		for (uint64_t i = 0; i < size(); i++) {
			new (&data[inc_mod(fidx+i)]) T{that.data[inc_mod(fidx+i)]};
		}
	}

	// move constructor logic
	void my_move(vector&& tmp) {
		this->data = tmp.data;
		this->capacity = tmp.capacity;
		this->fidx = tmp.fidx;
		this->eidx = tmp.eidx;
		this->unit = tmp.unit;

		// make the destruction of tmp harmless
		// since now both this->data and tmp.data
		// point to the same location
		tmp.data = nullptr; 
	}

	// destructor logic
	void destroy(void) {
		// if data is not nullptr
		if (data) {
			// destroy each elem individually
			for (uint64_t i = 0; i < size(); i++) {
				data[inc_mod(fidx+i)].~T();
			}

			// free the allocated space (no destructors)
			::operator delete(data); 
		}
	}

	void add_back_resize(const T& elem) {
		T* new_data = (T*) ::operator new((capacity+unit) * sizeof(T));

		// we should copy the elem 1st before moving the old_data to new_data,
		// because if elem is part of the given vector, the address may change
		// and result in wrong array. Another alternative is to taking a tmp
		// var and copy construct the elem into it and then after resizing (and copying)
		// is done, move it to the appropriate place. however, the approach implemented
		// here has one move less overhead.

		// copy construct the elem first, before moving elems to the new array.
		new (&new_data[capacity]) T{elem}; 

		for (uint64_t i = 0; i < size(); i++) {
			new (&new_data[i+1]) T{std::move(data[inc_mod(fidx+i)])};
		}

		destroy();

		fidx = 0;
		eidx = capacity;
		capacity += unit;
		unit *= 2;
		data = new_data;
	}

	void add_front_resize(const T& elem) {
		T* new_data = (T*) ::operator new((capacity+unit) * sizeof(T));
	
		// copy construct the elem first, before moving elems to the new array.
		new (&new_data[1]) T{elem};

		for (uint64_t i = 0; i < size(); i++) {
			new (&new_data[i+2]) T{std::move(data[inc_mod(fidx+i)])};
		}

		destroy();

		fidx = 0;
		eidx = capacity;
		capacity += unit;
		unit *= 2;
		data = new_data;
	}

	// resizing logc
	void resize(void) {
		T* new_data = (T*) ::operator new((capacity+unit) * sizeof(T));

		for (uint64_t i = 0; i < size(); i++) {
			new (&new_data[i+1]) T{std::move(data[inc_mod(fidx+i)])};
		}

		destroy();

		fidx=0;
		eidx=capacity-1;
		capacity += unit;
		unit *= 2;

		this->data=new_data;
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
			data = (T*) ::operator new((size+1) * sizeof(T));
			
			for (uint64_t i=1; i < size+1; i++) {
				new (&data[i]) T{};
			}

			fidx = 0;
			eidx = size;
			capacity = size+1;
			unit = INITIAL_UNIT;
		}
	}

	// Copy constructor.
	vector(vector const& that) {
		copy(that);
	}

	// move constructor
	vector(vector&& tmp) {
		my_move(std::move(tmp));
	}

	// copy assignment operator
	vector<T>& operator=(vector const& rhs) {
		if (this != &rhs) {
			destroy();
			copy(rhs);
		}

		return *this;
	}

	// move assignment operator
	vector<T>& operator=(vector&& rhs) {
		std::swap(this->data, rhs.data);

		this->capacity = rhs.capacity;
		this->fidx = rhs.fidx;
		this->eidx = rhs.eidx;
		this->unit = rhs.unit;

		return *this;
	}

	// returns the number of elements in the array
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

		if (!empty_spots()) {
			add_back_resize(elem);
		} else {
			eidx = inc_mod(eidx);
			new (&this->data[eidx]) T{elem};
		}
	}

	void push_back(T&& elem) {
		allocIfNull();

		// If there is no empty spot.
		if (!empty_spots()) {
			resize();
		}

		eidx = inc_mod(eidx);
		new (&this->data[eidx]) T{std::move(elem)};
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
			add_front_resize(elem);
		} else {
			new (&this->data[fidx]) T{elem};
			fidx = dec_mod(fidx);
		}
	}

	void push_front(T&& elem) {
		std::cout << "**in move push_front\n";
		allocIfNull();
		
		if (!empty_spots()) {
			resize();
		}

		new (&this->data[fidx]) T{std::move(elem)};

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
