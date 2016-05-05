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
using std::declval;

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

template <typename T>
struct sqrt_type {
	static constexpr int rank = SRank<T>::value;

	static constexpr int promoted_rank = rank < 4 ? 3 : 5;

	using type = typename SType<promoted_rank>::type;
};

template <typename T1, typename T2>
using ChooseType = typename choose_type<T1, T2>::type;

template <typename T>
using SqrtType = typename sqrt_type<T>::type;

template <typename S1Type, typename S2Type, typename Op>
class Expression;

template <typename E>
struct Wrap; 

template <typename T>
class Valarray : public epl::vector<T> {
	using Same = Valarray<T>; // defining Same type
public:
	using epl::vector<T>::vector; // to inherit all the constructors

	~Valarray(void) = default;

	Valarray(void) : epl::vector<T>() {}

	template <typename S1Type, typename S2Type, typename Op>
	Valarray(Wrap<Expression<S1Type, S2Type, Op>> const& expression) : Valarray<T>(expression.size()) {
		Expression<S1Type, S2Type, Op> const& expr{ expression };

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

	// using iterator = typename Valarray<value_type>::iterator;
	// using const_iterator = typename Valarray<value_type>::const_iterator;

	Expression(S1Type const& l, S2Type const& r, Op _op = Op{}) : 
		left{ l }, right(r), op(_op) {}

	~Expression(void) = default;

	Expression<S1Type, S2Type, Op>& operator=(const Expression<S1Type, S2Type, Op>& that) = default;

	uint64_t size(void) const {
		return std::min(left.size(), right.size()); 
	}

	template <typename N, typename... not_used>
	auto operator_bracket(uint64_t k, not_used...) const {
		return op((value_type) left[k], (value_type) right[k]);
	}

	// S1Type::value_type should also work as return type and casting...
	template <typename N, typename test=decltype(declval<N>().foo(0))>
	auto operator_bracket(uint64_t k) const {
		return op((value_type) left[k]);
	}

	template<typename K = void>
	auto operator[](uint64_t k) const {
		return operator_bracket<RightType>(k);
	}

	void print(std::ostream& out) const {
		const char* pref = "";

		for (uint64_t i = 0; i < this->size(); i++) {
			out << pref << (*this)[i];
			pref = ", ";
		}
	}

	template <bool NonConst>
	class Iterator;

	using iterator = Iterator<true>;
	using const_iterator = Iterator<false>;

	// non-const to const is valid
	template <bool NonConst>
	class Iterator {
	private:
		using vtype = const Expression<S1Type, S2Type, Op>&;
		uint64_t itr_idx;
		//uint64_t vnumber;
		//uint64_t anumber;
		vtype v;

		/*void check_iterator_validity(void) const {
			// if the version number has not changed,
			// Iterator is valid (even if it is out of range!)
			if (vnumber == v.vnumber) {
				return;
			}

			// Iterator references a position that does 
			// not exist due to modifications in vector
			// (e.g. out-of-bounds position, because of a pop_back)
			if (!v.is_idx_in_range(itr_idx)) {
				throw invalid_iterator(invalid_iterator::SeverityLevel::SEVERE);
			}

			// if the Iterator reference a position that is in-bounds, but 
			// the memory location for that position may have been changed 
			// (e.g., a reallocation has been performed because of a push_back, 
			// or a new assignment has been performed to the vector), then the 
			// exception you throw must have the level MODERATE.
			if (anumber != v.anumber) {
				throw invalid_iterator(invalid_iterator::SeverityLevel::MODERATE);
			}

			// invalidated for other reasons
			throw invalid_iterator(invalid_iterator::SeverityLevel::MILD);
		}

		void cmp_opt_iterators_validity(Iterator const& rhs) const {
			this->check_iterator_validity();
			rhs.check_iterator_validity();
		}*/
	
	public:
		using value_type = typename Expression<S1Type, S2Type, Op>::value_type;
		using reference = const value_type&;
		using pointer = const value_type*;
		using difference_type = uint64_t;
		using iterator_category = std::random_access_iterator_tag;

		// need a default constructor due to this link:
		// http://www.cplusplus.com/reference/iterator/RandomAccessIterator/
		// the behavior should be undefined though
		Iterator(void) = default;

		// construct an Iterator, with itr_idx equal to vec.fidx
		Iterator(vtype vec) : itr_idx(0), v(vec)  {}

		// construct an Iterator, with itr_idx equal to idx
		Iterator(vtype vec, uint64_t idx) : itr_idx(idx), v(vec)  {}

		// construct an Iterator, with itr_idx equal to vec.eidx + 1 (STL convention [a, b))
		Iterator(vtype vec, bool dummy) : itr_idx(vec.size()), v(vec) {}

		// copy constuctor
		Iterator(Iterator const& rhs) = default;

		// copy assignment operator
		Iterator& operator=(Iterator const& rhs) = default;

		operator const_iterator() const {
			return const_iterator{this->v, this->itr_idx};
		}

		// default destructor
		~Iterator(void) = default;

		value_type operator*(void) const {
			return v[itr_idx];
		}

		pointer operator->(void) const { 
			return &v[itr_idx]; 
		}

		// itr += a
		Iterator& operator+=(uint64_t num) {
			itr_idx += num;

			return *this;
		}

		// itr + a
		Iterator operator+(uint64_t num) const {
			Iterator itr { *this };
			itr += num;

			return itr;
		}

		// itr -= a
		Iterator& operator-=(uint64_t num) {
			itr_idx -= num;

			return *this;
		}

		// itr - a
		Iterator operator-(uint64_t num) const {
			Iterator itr { *this };
			itr -= num;

			return itr;
		}

		// itr1 - itr2
		// if the vectors in "this" and rhs are different, 
		// it's undefined behavior
		uint64_t operator-(Iterator const& rhs) const {
			return itr_idx - rhs.itr_idx;
		}

		// itr[] -> *(itr + num)
		reference operator[](uint64_t num) const {
			return *(*this + num);
		}

		// prefix ++
		Iterator& operator++(void) {
			itr_idx++;
			return *this;
		}

		// postfix ++
		Iterator operator++(int dummy) {
			Iterator itr { *this };
			
			operator++();

			return itr;
		}

		// prefix --
		Iterator& operator--(void) {
			itr_idx--;
			return *this;
		}

		// postfix --
		Iterator operator--(int dummy) {
			Iterator itr { *this };

			operator--();

			return itr;
		}

		// a == b
		bool operator==(Iterator const& rhs) const { 
			return itr_idx == rhs.itr_idx;
		}

		// a != b -> !(a == b)
		bool operator!=(Iterator const& rhs) const {
			return !(*this == rhs);
		}

		// a < b
		bool operator<(Iterator const& rhs) const {
			return itr_idx < rhs.itr_idx;
		}

		// a > b -> !(a < b) && a != b
		bool operator>(Iterator const& rhs) const {
			return (rhs < *this) && (rhs != *this);
		}

		// a <= b -> !(a > b)
		bool operator<=(Iterator const& rhs) const {
			return !(*this > rhs);
		}

		// a >= b -> !(a < b)
		bool operator>=(Iterator const& rhs) const {
			return !(*this < rhs);
		}
	};

	/*iterator begin(void) {
		return Iterator<true>(*this);
	}*/

	const_iterator begin(void) const {
		return Iterator<false>(*this);
	}

	/*iterator end(void) {
		return Iterator<true>(*this, true);
	}*/

	const_iterator end(void) const {
		return Iterator<false>(*this, true);
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

struct UnaryWrap {
	bool unary;

	using value_type = int; // lowest type

	void foo(int) {}

	UnaryWrap(void) = default;

	uint64_t size(void) const { 
		return std::numeric_limits<uint64_t>::max(); // infinity
	}
};

template<typename T>
struct my_sqrt {
	SqrtType<T> operator()(T const& arg) const {
	    return std::sqrt((SqrtType<T>)arg);
	}
};

template <typename E>
struct Wrap : public E {
	using E::E;
	Wrap(void) : E() {}
	Wrap(E const& e) : E(e) {}
	~Wrap(void) = default;
	using value_type = typename E::value_type;

	template<typename S>
	Wrap<E>& operator=(Wrap<S> const& rhs) {
		S const& right { rhs };
		uint64_t size = std::min(this->size(), right.size());

		for (uint64_t i = 0; i < size; i++) {
			(*this)[i] = right[i];
		}
		
		return *this;
	}

	template<typename Op>
	Wrap<Expression<E, UnaryWrap, Op>> 
	apply(Op op) {
		E const& left{ *this };

		Expression<E, UnaryWrap, Op> result{ left, UnaryWrap{}, op };
		return Wrap<Expression<E, UnaryWrap, Op>> { result };
	}

	Wrap<Expression<E, UnaryWrap, my_sqrt<value_type>>> 
	sqrt(void) {
		E const& left{ *this };

		Expression<E, UnaryWrap, my_sqrt<value_type>> result{ left, UnaryWrap{} };
		return Wrap<Expression<E, UnaryWrap, my_sqrt<value_type>>> { result };
	}

	value_type sum(void) {
		return accumulate(std::plus<>{});
	}

	template<typename Op>
	value_type accumulate(Op op) {
		if (this->size() < 1) {
			return 0;
		}

		value_type result = (*this)[0];

		for (uint64_t i = 1; i < this->size(); i++) {
			result = op((value_type) result, (value_type) (*this)[i]);
		}

		return result;
	}

	/*typename E::const_iterator begin(void) const {
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
	}*/
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