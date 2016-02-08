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

	void my_move(vector&& tmp) {
		// cout << "move!\n";

		this->data = tmp.data;
		this->capacity = tmp.capacity;
		this->fidx = tmp.fidx;
		this->eidx = tmp.eidx;
		this->unit = tmp.unit;
		tmp.data = nullptr;
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

	void resize(void) {
		T* new_data = (T*) ::operator new((capacity+unit) * sizeof(T));

		for (uint64_t i = 0; i < size(); i++) {
			new (&new_data[i+1]) T{std::move(data[inc_mod(fidx+i)])};
		}

		fidx=0;
		eidx=capacity-1;
		capacity += unit;
		unit *= 2;

		destroy();
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

	vector(vector&& tmp) {
		my_move(std::move(tmp));
	}

	vector<T>& operator=(vector const& rhs) {
		if (this != &rhs) {
			destroy();
			copy(rhs);
		}

		return *this;
	}

	vector<T>& operator=(vector&& rhs) {
		std::swap(this->data, rhs.data);

		this->capacity = rhs.capacity;
		this->fidx = rhs.fidx;
		this->eidx = rhs.eidx;
		this->unit = rhs.unit;

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
			resize();
		}

		eidx = inc_mod(eidx);

		new (&this->data[eidx]) T{elem};
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
			resize();
		}

		new (&this->data[fidx]) T{elem};

		fidx = dec_mod(fidx);
	}

	void push_front(T&& elem) {
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
