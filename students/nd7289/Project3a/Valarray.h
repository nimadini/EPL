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

using std::complex;

namespace epl {

template <typename T>
class Valarray;

template <typename>
struct SRank;

template <> struct SRank<int> { static constexpr int value = 1; };
template <> struct SRank<float> { static constexpr int value = 2; };
template <> struct SRank<double> { static constexpr int value = 3; };
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

template <typename S1Type, typename S2Type, typename Op>
class Expression;

template <typename E>
struct Wrap; 

template <typename T>
class Valarray : public epl::vector<T> {
	using Same = Valarray<T>; // defining Same type
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

	// TODO: why should I define, when I declare the other valarray?
	Valarray(void) : epl::vector<T>() {}

	// TODO: if valarray of T or S1Type, S2Type???
	template <typename S1Type, typename S2Type, typename Op>
	Valarray(Expression<S1Type, S2Type, Op> const& expr) {
		new (this) Valarray<ChooseType<typename S1Type::value_type, typename S2Type::value_type>>(expr.size());

		for (uint64_t i=0; i < expr.size(); i++) {
			(*this)[i] = expr[i];
		}
	}

	void print(std::ostream& out) const {
		const char* pref = "";
		for (const auto& val : *this) {
			out << pref << val;
			pref = ", ";
		}
	}
};

/*
template <typename E>
struct Wrap; */

template <typename T> struct choose_ref {
	using type = T;
};

// TODO HERE: template<typename T> struct choose_ref<Wrap<Valarray<T>>> {

template<typename T> struct choose_ref<Valarray<T>> {
	using type = Valarray<T> const&;	
};

template <typename T> using ChooseRef = typename choose_ref<T>::type;

// valarray proxy
template <typename S1Type, typename S2Type, typename Op>
class Expression {
	using LeftType = ChooseRef<S1Type>;
	using RightType = ChooseRef<S2Type>;
	LeftType left;
	RightType right;
	Op op;
public:
	using value_type = ChooseType<typename S1Type::value_type, typename S2Type::value_type>;

	using iterator = typename Valarray<value_type>::iterator;
	using const_iterator = typename Valarray<value_type>::const_iterator;

	Expression(S1Type const& l, S2Type const& r) : 
		left{ l }, right(r), op(Op{}) {}

	uint64_t size(void) const {
		return std::min(left.size(), right.size()); 
	}

	value_type operator[](uint64_t k) const {
		return op((value_type) left[k], (value_type) right[k]);
	}

	// TODO:: operator[] non-const???

	// TODO:: why do we need an assignment operator?

	// issues: https://piazza.com/class/ik5telvhcgio3?cid=129

	Expression<S1Type, S2Type, Op>& operator=(const Expression<S1Type, S2Type, Op>& that) = default;

	void print(std::ostream& out) const {
		const char* pref = "";

		for (uint64_t i = 0; i < this->size(); i++) {
			out << pref << (*this)[i];
			pref = ", ";
		}
	}
};

template<typename T>
class Scalar {
	T value;
public:
	using value_type = T;
	Scalar(T val) : value(val) {}

	uint64_t size(void) const { 
		return std::numeric_limits<uint64_t>::max(); // infinity
	}

	T operator[](uint64_t k) const {
		return value;
	}

	// TODO: Sup bro?
	void print(std::ostream& out) const {
		out << value;
	}
};

template <typename E>
struct Wrap : public E {
	using E::E;
	Wrap(void) : E() {}
	Wrap(E const& e) : E(e) {}
	~Wrap(void) = default;

	template<typename S>
	Wrap<E>& operator=(Wrap<S> const& rhs) {
		S const& right { rhs };
		uint64_t size = std::min(this->size(), right.size());

		for (uint64_t i = 0; i < size; i++) {
			(*this)[i] = right[i];
		}
		
		return *this;
	}

	typename E::const_iterator begin(void) const {
		return this->E::begin();
	}

	typename E::iterator begin(void) {
		return this->E::begin();
	}

	typename E::const_iterator end(void) const {
		return this->E::end();
	}

	typename E::iterator end(void) {
		return this->E::end();
	}
};

template<typename T>
using valarray = Wrap<Valarray<T>>;

// operator +
template<typename S1, typename S2>
Wrap<Expression<S1, S2, std::plus<>>>
operator+(Wrap<S1> const& lhs, Wrap<S2> const& rhs) {
	S1 const& left{ lhs };
	S2 const& right{ rhs };

	Expression<S1, S2, std::plus<>> result{ left, right };
	return Wrap<Expression<S1, S2, std::plus<>>> { result };
}

template <typename T, typename S>
Wrap<Expression<Scalar<T>, S, std::plus<>>>
operator+(T lhs, Wrap<S> const& rhs) {
	return Wrap<Expression<Scalar<T>, S, std::plus<>>>{Scalar<T>{ lhs }, rhs};
}

template <typename S, typename T>
Wrap<Expression<S, Scalar<T>, std::plus<>>>
operator+(Wrap<S> const& lhs, T rhs) {
	return Wrap<Expression<S, Scalar<T>, std::plus<>>>{lhs, Scalar<T>{ rhs }};
}

// operator -
template<typename S1, typename S2>
Wrap<Expression<S1, S2, std::minus<>>> 
operator-(Wrap<S1> const& lhs, Wrap<S2> const& rhs) {
	S1 const& left{ lhs };
	S2 const& right{ rhs };

	Expression<S1, S2, std::minus<>> result{ left, right };
	return Wrap<Expression<S1, S2, std::minus<>>> { result };
}

template <typename T, typename S>
Wrap<Expression<Scalar<T>, S, std::minus<>>>
operator-(T lhs, Wrap<S> const& rhs) {
	return Wrap<Expression<Scalar<T>, S, std::minus<>>>{Scalar<T>{ lhs }, rhs};
}

template <typename S, typename T>
Wrap<Expression<S, Scalar<T>, std::minus<>>>
operator-(Wrap<S> const& lhs, T rhs) {
	return Wrap<Expression<S, Scalar<T>, std::minus<>>>{lhs, Scalar<T>{ rhs }};
}

// operator *
template<typename S1, typename S2>
Wrap<Expression<S1, S2, std::multiplies<>>>
operator*(Wrap<S1> const& lhs, Wrap<S2> const& rhs) {
	S1 const& left{ lhs };
	S2 const& right{ rhs };

	Expression<S1, S2, std::multiplies<>> result{ left, right };
	return Wrap<Expression<S1, S2, std::multiplies<>>> { result };
}

template <typename T, typename S>
Wrap<Expression<Scalar<T>, S, std::multiplies<>>>
operator*(T lhs, Wrap<S> const& rhs) {
	return Wrap<Expression<Scalar<T>, S, std::multiplies<>>>{Scalar<T>{ lhs }, rhs};
}

template <typename S, typename T>
Wrap<Expression<S, Scalar<T>, std::multiplies<>>>
operator*(Wrap<S> const& lhs, T rhs) {
	return Wrap<Expression<S, Scalar<T>, std::multiplies<>>>{lhs, Scalar<T>{ rhs }};
}

// operator /
template<typename S1, typename S2>
Wrap<Expression<S1, S2, std::divides<>>>
operator/(Wrap<S1> const& lhs, Wrap<S2> const& rhs) {
	S1 const& left{ lhs };
	S2 const& right{ rhs };

	Expression<S1, S2, std::divides<>> result{ left, right };
	return Wrap<Expression<S1, S2, std::divides<>>> { result };
}

template <typename T, typename S>
Wrap<Expression<Scalar<T>, S, std::divides<>>> 
operator/(T lhs, Wrap<S> const& rhs) {
	return Wrap<Expression<Scalar<T>, S, std::divides<>>>{Scalar<T>{ lhs }, rhs};
}

template <typename S, typename T>
Wrap<Expression<S, Scalar<T>, std::divides<>>>
operator/(Wrap<S> const& lhs, T rhs) {
	return Wrap<Expression<S, Scalar<T>, std::divides<>>>{lhs, Scalar<T>{ rhs }};
}

// unary -
template <typename S>
Wrap<Expression<Scalar<int>, S, std::minus<>>>
operator-(Wrap<S> const& varray) {
	return Wrap<Expression<Scalar<int>, S, std::minus<>>>{Scalar<int>{ 0 }, varray};
}

template <typename T>
std::ostream& operator<<(std::ostream& out, Wrap<T> const& vec) {
	vec.print(out);
	return out;
}

}

#endif /* _Valarray_h */