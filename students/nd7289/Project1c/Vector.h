#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <iostream>

#define INITIAL_UNIT 8

using std::cout;

//Utility gives std::rel_ops which will fill in relational
//iterator operations so long as you provide the
//operators discussed in class.  In any case, ensure that
//all operations listed in this website are legal for your
//iterators:
//http://www.cplusplus.com/reference/iterator/RandomAccessIterator/
using namespace std::rel_ops;

namespace epl {

class invalid_iterator {
	public:
	enum SeverityLevel {SEVERE,MODERATE,MILD,WARNING};
	SeverityLevel level;	

	invalid_iterator(SeverityLevel level = SEVERE){ this->level = level; }
	virtual const char* what() const {
	    switch (level) {
	      case WARNING:   return "Warning"; // not used in Spring 2015
	      case MILD:      return "Mild";
	      case MODERATE:  return "Moderate";
	      case SEVERE:    return "Severe";
	      default:        return "ERROR"; // should not be used
	    }
	}
};

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

	// Note: vnumber and anumber should not be copied/moved in copy and move logics
	// basically they are created once during the lifetime of an object and will
	// only be incremented at certain places
	uint64_t vnumber;	// version number (used in iterator)
	uint64_t anumber;	// allocation number (used in iterator), 
					 	// re-assignment to vector is also considered an allocation

	// init a vector: data would be null, fidx
	// and eidx both point to the 1st elem
	void init(void) {
		data = nullptr;
		capacity = INITIAL_UNIT;
		fidx = eidx = 0;
		unit = INITIAL_UNIT;
		vnumber = 0;
		anumber = 0;
	}

	bool same_version(vector const& v) {
		return vnumber == v.vnumber;
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

		anumber++;
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

		anumber++;
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

		anumber++;
	}

	void allocIfNull(void) {
		// Check if data has not been allocated (nullptr).
		if (!data) {
			data = (T*) ::operator new(capacity * sizeof(T));
		}

		anumber++;
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

	bool is_idx_in_range(uint64_t idx) const {
		if (eidx >= fidx) {
			return idx >= fidx && idx <= eidx;
		}

		return idx >= fidx || idx <= eidx;
	}

	// returns the absolute index
	uint64_t abs_idx(uint64_t idx) const {
		if (idx >= fidx) {
			return idx - fidx;
		}
		return idx + capacity - fidx;
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
			anumber++;
		}

		return *this;
	}

	// move assignment operator
	vector<T>& operator=(vector&& rhs) {
		std::swap(this->data, rhs.data);
		std::swap(this->capacity, rhs.capacity);
		std::swap(this->fidx, rhs.fidx);
		std::swap(this->eidx, rhs.eidx);
		std::swap(this->unit, rhs.unit);

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

		vnumber++;
	}

	void push_back(T&& elem) {
		allocIfNull();

		// If there is no empty spot.
		if (!empty_spots()) {
			resize();
		}

		eidx = inc_mod(eidx);
		new (&this->data[eidx]) T{std::move(elem)};

		vnumber++;
	}

	void pop_back(void) {
		// If array is empty.
		if (!size()) {
			throw std::out_of_range("subscript out of range");
		}

		this->data[eidx].~T();
		eidx = dec_mod(eidx);

		vnumber++;
	}

	void push_front(const T& elem) {
		allocIfNull();
		
		if (!empty_spots()) {
			add_front_resize(elem);
		} else {
			new (&this->data[fidx]) T{elem};
			fidx = dec_mod(fidx);
		}

		vnumber++;
	}

	void push_front(T&& elem) {
		allocIfNull();
		
		if (!empty_spots()) {
			resize();
		}

		new (&this->data[fidx]) T{std::move(elem)};

		fidx = dec_mod(fidx);

		vnumber++;
	}

	void pop_front(void) {
		// If array is empty.
		if (!size()) {
			throw std::out_of_range("subscript out of range");
		}

		fidx = inc_mod(fidx);
		this->data[fidx].~T();

		vnumber++;
	}

	void print(void) {
		for (uint64_t i = 0; i < size(); i++) {
			cout << data[(fidx+1+i) % capacity] << " ";
		}

		cout << "\n";
	}

	class iterator;

    friend class iterator;

	class iterator {
	private:
		uint64_t itr_idx;
		uint64_t vnumber;
		uint64_t anumber;
		vector<T> const& v;

		void check_iterator_validity(void) const {
			// iterator references a position that does 
			// not exist, i.e., the position is out-of-bounds
			if (!v.is_idx_in_range(itr_idx)) {
				throw invalid_iterator(invalid_iterator::SeverityLevel::SEVERE);
			}

			// if the iterator reference a position that is in-bounds, but 
			// the memory location for that position may have been changed 
			// (e.g., a reallocation has been performed because of a push_back, 
			// or a new assignment has been performed to the vector), then the 
			// exception you throw must have the level MODERATE.
			if (anumber != v.anumber) {
				throw invalid_iterator(invalid_iterator::SeverityLevel::MODERATE);
			}

			// invalidated for other reasons
			if (vnumber != v.vnumber) {
				throw invalid_iterator(invalid_iterator::SeverityLevel::MILD);
			}
		}

		void cmp_opt_iterators_validity(iterator const& rhs) const {
			this->check_iterator_validity();
			rhs.check_iterator_validity();
		}
	
	public:
		using value_type = T;
		using reference = T&;
		using pointer = T*;
		using difference_type = uint64_t;
		using iterator_category = std::random_access_iterator_tag;

		// construct an iterator, with itr_idx equal to vec.fidx
		iterator(vector const& vec) : v(vec), itr_idx(vec.fidx), vnumber(vec.vnumber), anumber(vec.anumber)  {}

		// construct an iterator, with itr_idx equal to vec.eidx + 1 (STL convention [a, b))
		iterator(vector const& vec, bool dummy) : v(vec), itr_idx(vec.eidx), vnumber(vec.vnumber), anumber(vec.anumber) {}

		// copy constructor
		iterator(iterator const& rhs) = default;

		// default destructor
		~iterator(void) = default;

		reference operator*(void) const {
			check_iterator_validity();

			return v[itr_idx];
		}

		pointer operator->(void) const { 
			check_iterator_validity();

			return &v[itr_idx]; 
		}

		// a += b
		iterator& operator+=(uint64_t num) {
			itr_idx = (itr_idx + num) % v.capacity;

			return *this;
		}

		// a + b
		iterator operator+(uint64_t num) const {
			iterator itr { *this };
			itr += num;

			return itr;
		}

		// prefix ++
		iterator& operator++(void) {
			check_iterator_validity();

			itr_idx = v.inc_mod(itr_idx);
			return *this;
		}

		// postfix ++
		iterator operator++(int dummy) {
			check_iterator_validity();

			iterator itr { *this };
			
			operator++();

			return itr;
		}

		// prefix --
		iterator& operator--(void) {
			check_iterator_validity();

			itr_idx = v.dec_mod(itr_idx);
			return *this;
		}

		// postfix --
		iterator operator--(int dummy) {
			check_iterator_validity();

			iterator itr { *this };

			operator--();

			return itr;
		}

		// a == b
		bool operator==(iterator const& rhs) const { 
			cmp_opt_iterators_validity(rhs);

			return itr_idx == rhs.itr_idx;
		}

		// a != b -> !(a == b)
		bool operator!=(iterator const& rhs) const {
			cmp_opt_iterators_validity(rhs);

			return !(*this == rhs);
		}

		// a < b
		bool operator<(iterator const& rhs) const {
			cmp_opt_iterators_validity(rhs);

			return v.abs_idx(itr_idx) < rhs.v.abs_idx(rhs.itr_idx);
		}

		// a > b -> !(a < b) && a != b
		bool operator>(iterator const& rhs) const {
			cmp_opt_iterators_validity(rhs);

			return (rhs < *this) && (rhs != *this);
		}

		// a <= b -> !(a > b)
		bool operator<=(iterator const& rhs) const {
			cmp_opt_iterators_validity(rhs);

			return !(*this > rhs);
		}

		// a >= b -> !(a < b)
		bool operator>=(iterator const& rhs) const {
			cmp_opt_iterators_validity(rhs);

			return !(*this < rhs);
		}
	};

	// iterator pointing to beginning of the vector
	iterator begin(void) {
		return iterator(*this);
	}

	// iterator pointing to end of the vector
	iterator end(void) {
		return iterator(*this, true);
	}
};

} //namespace epl

#endif
