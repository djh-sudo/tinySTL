#ifndef STL_CONSTRUCT_H
#define STL_CONSTRUCT_H
#include <new.h>
#include "stl_iterator_base.h"
#include "type_traits.h"

namespace stl {
	template<typename T1, typename T2>
	inline void Construct(T1* p, const T2 value) {
		new ((void*)p) T1(value);
	}

	template<typename T1>
	inline void Construct(T1* p) {
		new ((void*)p) T1();
	}
	// special 
	inline void Construct(int* p) {}
	inline void Construct(unsigned int* p) {}
	inline void Construct(char* p) {}
	inline void Construct(unsigned char* p) {}
	inline void Construct(double* p) {}
	inline void Construct(float* p) {}
	inline void Construct(long* p) {}

	inline void Construct(int* p, const int value) { *p = value; }
	inline void Construct(unsigned int* p, const unsigned int value) { *p = value; }
	inline void Construct(char* p, const char value) { *p = value; }
	inline void Construct(unsigned char* p, const unsigned char value) { *p = value; }
	inline void Construct(float* p, const float value) { *p = value; }
	inline void Construct(double* p, const double value) { *p = value; }

	template<typename T1>
	inline void Destroy(T1* p) {
		p->~T1();
	}

	template<typename ForwardIterator>
	void destroy_aux(ForwardIterator first, ForwardIterator last, _false_type) {
		for (; first != last; ++first) {
			destroy(&(*first));
		}
	}

	template<class ForwardIterator>
	inline void destroy_aux(ForwardIterator f, ForwardIterator l, _true_type) {}


	template<typename ForwardIterator, typename Tp>
	inline void _destroy(ForwardIterator first, ForwardIterator last, Tp*) {
		typedef typename _type_traits<Tp>::has_trivial_destructor
			trivial_destructor;
		destroy_aux(first, last, trivial_destructor());
	}

	template<typename ForwardIterator>
	inline void Destroy(ForwardIterator first, ForwardIterator last) {
		_destroy(first, last, VALUE_TYPE(first));
	}
 
	inline void Destroy(char*, char*) {}
	inline void Destroy(int*, int*) {}
	inline void Destroy(long*, long*) {}
	inline void Destroy(float*, float*) {}
	inline void Destroy(double*, double*) {}

	template<typename T1, typename T2>
	inline void construct(T1* p, const T2& value) {
		Construct(p, value);
	}

	template<typename T1>
	inline void construct(T1* p) {
		Construct(p);
	}

	template<typename Tp>
	inline void destroy(Tp* p) {
		Destroy(p);
	}

	template<typename ForwardIterator>
	inline void destroy(ForwardIterator first, ForwardIterator last) {
		Destroy(first, last);
	}
}
#endif