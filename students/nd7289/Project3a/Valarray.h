// Valarray.h

/* Put your solution in this file, we expect to be able to use
 * your epl::valarray class by simply saying #include "Valarray.h"
 *
 * We will #include "Vector.h" to get the epl::vector<T> class 
 * before we #include "Valarray.h". You are encouraged to test
 * and develop your class using std::vector<T> as the base class
 * for your epl::valarray<T>
 * you are required to submit your project with epl::vector<T>
 * as the base class for your epl::valarray<T>
 */

#ifndef _Valarray_h
#define _Valarray_h

#include <iostream>
#include <cstdint>
#include <algorithm>
#include <complex>
#include "Vector.h"


//using std::vector; // during development and testing
using epl::vector; // after submission

using std::complex;

namespace epl {

template <typename T>
class valarray;

template <typename Op, typename T>
void apply_op(valarray<T> & lhs, valarray<T> const& x, valarray<T> const& y, Op op = Op{}) {
	uint64_t size = std::min(x.size(), y.size());
	size = std::min(size, lhs.size()); // probably not needed
	for (uint64_t i = 0; i < size; i++) {
		lhs[i] = op(x[i], y[i]);
	}
}


template <typename>
struct SRank;

template <> struct SRank<int> { 
	static constexpr int value = 1;  
};
template <> struct SRank<float> { 
	static constexpr int value = 2; 
};
template <> struct SRank<double> { 
	static constexpr int value = 3; 
};
template <typename T> struct SRank<complex<T>> {
	static constexpr int value = SRank<T>::value + 2;
};

template <int>
struct SType;

template <> struct SType<1> { using type = int; };
template <> struct SType<2> { using type = float; };
template <> struct SType<3> { using type = double; };
template <> struct SType<4> { using type = complex<float>; };
template <> struct SType<5> { using type = complex<double>; };

template <typename T1, typename T2>
struct choose_type {
	static constexpr int t1_rank = SRank<T1>::value;
	static constexpr int t2_rank = SRank<T2>::value;

	static constexpr int max_rank = (t1_rank < t2_rank) ? t2_rank : t1_rank;
	static constexpr int min_rank = (t1_rank < t2_rank) ? t1_rank : t2_rank;

	static constexpr int rank = min_rank == 3 && max_rank == 4 ? 5 : max_rank;

	using type = typename SType<rank>::type;
};

template <typename T1, typename T2>
using ChooseType = typename choose_type<T1, T2>::type;

template <typename T1, typename T2>
ChooseType<T1, T2> bax(T1 const& x, T2 const& y) {
	/*if (x < y) { return y; }
	else { return x; }*/
	return x;
}

template <typename T>
class valarray : public vector<T> {
	using Same = valarray<T>; // defining Same type
public:
	using epl::vector<T>::vector; // to inherit all the constructors

	// changing the semantics of assignment operator
	Same& operator=(Same const& rhs) {
		// in this special case, not checking if-not-self only harms performance-wise, 
		// but it's stll correct since in the assignment operator of T, this case should 
		// be handled. In other words, (*this)[i] = rhs[i] takes care of it.
		if (this != &rhs) {
			uint64_t size = std::min(this->size(), rhs.size());

			for (uint64_t i = 0; i < size; i++) {
				(*this)[i] = rhs[i];
			}
		}
		return *this;
	}

	Same& operator+=(Same const& rhs) {
		apply_op<std::plus<void>>(*this, *this, rhs);
		return *this;
	}

	Same& operator-=(Same const& rhs) {
		apply_op<std::minus<void>>(*this, *this, rhs);
		return *this;
	}
};

template <typename T>
valarray<T> operator+(valarray<T> const& lhs, valarray<T> const& rhs) {
	valarray<T> result(std::min(lhs.size(), rhs.size()));
	apply_op<std::plus<void>>(result, lhs, rhs);
	return result;
}

template <typename T>
valarray<T> operator-(valarray<T> const& lhs, valarray<T> const& rhs) {
	valarray<T> result(std::min(lhs.size(), rhs.size()));
	apply_op<std::minus<void>>(result, lhs, rhs);
	return result;
}

template <typename T> struct choose_ref {
	using type = T;
};

template<typename T> struct choose_ref<valarray<T>> {
	using type = valarray<T> const&;	
};

template <typename T> using ChooseRef = typename choose_ref<T>::type;

template <typename S1Type, typename S2Type>
class Expression {
	using LeftType = ChooseRef<S1Type>;
	using RightType = ChooseRef<S2Type>;
	LeftType left;
	RightType right;
public:
	Expression(S1Type const& l, S2Type const& r) :
		left{ l }, right(r) {}

	uint64_t size(void) const { return left.size() + right.size(); }

	char operator[](uint64_t k) const {
		if (k < left.size()) { return left[k]; }
		else { return right[k - left.size()]; }
	}
};

template <typename T>
std::ostream& operator<<(std::ostream& out, const valarray<T>& varray) {
	const char* pref = "";
	for (const auto& val : varray) {
		out << pref << val;
		pref = ", ";
	}
	return out;
}

}

#endif /* _Valarray_h */

