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
	T* array_front;
	T* array_back;

	uint64_t fidx;
	uint64_t bidx;
	
	uint64_t fcap;
	uint64_t bcap;

	void init(void) {
		this->array_front=nullptr;
		this->array_back=nullptr;
		this->fidx=this->bidx=0;
		this->fcap=this->bcap=default_size;
	}

	void copy(vector const& that) {
		this->fidx=that.fidx;
		this->bidx=that.bidx;

		this->fcap=that.fcap;
		this->bcap=that.bcap;

		this->array_front=new T[fcap];
		this->array_back=new T[bcap];

		for (uint64_t i=0; i<fidx; i++) {
			this->array_front[i]=that.array_front[i];
		}

		for (uint64_t i=0; i<bidx; i++) {
			this->array_back[i]=that.array_back[i];
		}
	}

	void destroy(void) {
		destroy_front();
		destroy_back();
	}

	void destroy_front(void) {
		// Field data might be nullptr.

		if (this->array_front) {
			delete[] this->array_front;
		}
	}

	void destroy_back(void) {
		// Field data might be nullptr.

		if (this->array_back) {
			delete[] this->array_back;
		}
	}

	void allocIfNull(void) {
		// Check if data has not yet been initialized (nullptr).
		if (!this->array_front) {
			this->array_front=new T[fcap];
		}

		if (!this->array_back) {
			this->array_back=new T[bcap];
		}
	}

	uint64_t remaining_back(void) {
		return bcap-bidx-1;
	}

	uint64_t remaining_front(void) {
		return fcap-fidx-1;
	}

	uint64_t size_back(void) const {
		return bidx;
	}

	uint64_t size_front(void) const {
		return fidx;
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
			this->array_front=new T[size+1];
			this->array_back=new T[size+1];
			this->fidx=this->bidx=0;
			this->fcap=this->bcap=size+1;
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
		return size_back() + size_front();
	}

	T& operator[](uint64_t k) {
		if (k >= size()) {
			throw std::out_of_range("subscript out of range");
		}

		uint64_t idx=fidx;

		while (idx > 1 && k != 0) {
			idx -= 1;
			k -= 1;
		}

		if (k == 0) {
			return array_front[idx];
		} 

		if (fidx) {
			k-=1;
		}

		idx=1;

		while (idx <= bidx && k != 0) {
			idx+=1;
			k -= 1;
		}

		return array_back[idx];
	}



	/* const T& operator[](uint64_t k) const {
		if (k >= size()) {
			throw std::out_of_range("subscript out of range");			
		}

		return data[begin+k];
	} */

	void push_back(const T& elem) {
		allocIfNull();

		// If there is no empty spot at the back of the array.
		if (!remaining_back()) {
			bcap*=2; // Double the capacity.

			T* array_back_new=new T[bcap];

			for (uint64_t i=0; i <= bidx; i++) {
				array_back_new[i]=this->array_back[i];
			}

			destroy_back();
			this->array_back=array_back_new;
		}

		//T newElem{elem}; // Calling T's copy constructor???
		this->array_back[++bidx]=T{elem};
	}

	void pop_back(void) {
		// If there is no empty spot at the back of the array.

		if (bidx != 0) {
			this->array_back[bidx--].~T();
		} else if (fidx != 0)

		if (bidx == 0) {
			throw std::out_of_range("subscript out of range");
		}


		
	}

	void push_front(const T& elem) {
		allocIfNull();
		
		if (!remaining_front()) {
			fcap*=2;

			T* array_front_new=new T[fcap];

			for (uint64_t i=0; i <= fidx; i++) {
				array_front_new[i]=this->array_front[i];
			}

			destroy_front();
			this->array_front=array_front_new;
		}

		this->array_front[++fidx]=T{elem};
	}


	void print(void) {
		for (uint64_t i=fidx; i != 0; i-=1) {
			cout << array_front[i] << " ";
		}

		for (uint64_t i=1; i<=bidx; i+=1) {
			cout << array_back[i] << " ";
		}

		cout << "\n";
	}
};

} // Namespace epl.

#endif /* _Vector_h */
