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

#include "Vector.h"

//using std::vector; // during development and testing
using epl::vector; // after submission

namespace epl {

template <typename T>
class valarray : public vector<T> {
	using Same = valarray<T>; // defining Same type
public:
	using epl::vector<T>::vector; // to inherit all the constructors

	// changing the semantics of assignment operator
	Same& operator=(Same const& rhs) {
		if (this != &rhs) {
			uint64_t size = std::min(this->size(), rhs.size());

			for (uint64_t i = 0; i < size; i++) {
				(*this)[i] = rhs[i];
			}
		}
		return *this;
	}
};

}

#endif /* _Valarray_h */

