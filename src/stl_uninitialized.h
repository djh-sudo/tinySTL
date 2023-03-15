#ifndef STL_UNINITIALIZED_H
#define STL_UNINTIALIZED_H

#include "string.h"
#include "stl_config.h"
#include "type_traits.h"
#include "stl_iterator.h"
#include "stl_pair.h"
#include "stl_algobase.h"

NAMESPACE_START

template<typename InputIter, typename ForwardIter>
inline ForwardIter 
_uninitialized_copy_aux(InputIter first, InputIter last, 
	                    ForwardIter result, _true_type) {
	return copy(first, last, result);
}

template<typename InputIter, typename ForwardIter>
inline ForwardIter
_uninitialized_copy_aux(InputIter first, InputIter last,
	                    ForwardIter result, _false_type) {
	ForwardIter cur = result;
	for (; first != last; ++first, ++cur) {
		Construct(&(*cur), (*first));
	}
	return cur;
}
template<typename InputIter, typename ForwardIter, typename Tp>
inline ForwardIter
_uninitialized_copy(InputIter first, InputIter last,
	                    ForwardIter result, Tp*) {
	typedef typename _type_traits<Tp>::is_POD_type pod_type;
	return _uninitialized_copy_aux(first, last, result, pod_type());
}
template<typename InputIter, typename ForwardIter>
inline ForwardIter
uninitialized_copy(InputIter first, InputIter last, ForwardIter result) {
	return _uninitialized_copy(first, last, result, VALUE_TYPE(first));
}
// specialized
inline char* uninitialized_copy(const char* first, const char* last, 
	                            char* result) {
	::memmove(result, first, (size_t)(last - first));
	return result + (last - first);
}

inline int* uninitialized_copy(const int* first, const int* last,
	                            int* result) {
	::memmove(result, first, (size_t)(last - first));
	return result + (last - first);
}

// copy_n
template<typename InputIter, typename Size, typename ForwardIter>
pair<InputIter,ForwardIter>
_uninitialized_copy_n(InputIter first, Size count, 
	                    ForwardIter result, input_iterator_tag) {
	ForwardIter cur = result;
	for (; count > 0; --count, ++first) {
		Construct(&(*cur), *first);
	}
	return pair<InputIter, ForwardIter>(first, cur);
}

template<typename RAIter,typename Size,typename ForwardIter>
inline pair<RAIter,ForwardIter>
_uninitialized_copy_n(RAIter first, Size count, 
	                  ForwardIter result, random_access_iterator_tag) {
	RAIter last = first + count;
	return pair<RAIter, ForwardIter>(last,
		                             uninitialized_copy(first, last, result));
}
template<typename InputIter, typename Size, typename ForwardIter>
inline pair<InputIter, ForwardIter>
uninitialized_copy_n(InputIter first, Size count, ForwardIter result) {
	return _uninitialized_copy_n(first, count, result, ITERATOR_CATEGORY(first));
}
// fill n
template<typename ForwardIter, typename Tp>
inline void _uninitialized_fill_aux(ForwardIter first, ForwardIter last,
	                               const Tp&x, _true_type){
	fill(first, last, x);
}

template<typename ForwardIter, typename Tp>
inline void _uninitialized_fill_aux(ForwardIter first, ForwardIter last,
	                               const Tp&x, _false_type){
	ForwardIter cur = first;
	for (; cur != last; ++cur) {
		Construct(&(*cur), x);
	}
}

template<typename ForwardIter, typename Tp, typename T>
inline void _uninitialized_fill(ForwardIter first, ForwardIter last,
	                            Tp&x, T*){
	typedef typename _type_traits<T>::is_POD_type pod_type;
	return _uninitialized_fill_aux(first, last, x, pod_type());
}

template<typename ForwardIter, typename Tp>
inline void uninitialized_fill(ForwardIter first, ForwardIter last, Tp& x) {
	return _uninitialized_fill(first, last, x, VALUE_TYPE(first));
}

template<typename ForwardIter, typename Size, typename Tp>
inline ForwardIter
_uninitialized_fill_n_aux(ForwardIter first, Size n, const Tp& x, _true_type) {
	return fill_n(first, n, x);
}
template<typename ForwardIter, typename Size, typename Tp>
inline ForwardIter
_uninitialized_fill_n_aux(ForwardIter first, Size n, const Tp& x, _false_type) {
	ForwardIter cur = first;
	for (; n > 0; --n, ++cur) {
		Construct(&(*cur), x);
	}
	return cur;
}
template<typename ForwardIter, typename Size, typename Tp, typename T>
inline ForwardIter
_uninitialized_fill_n(ForwardIter first, Size n, const Tp& x, T*) {
	typedef typename _type_traits<T>::is_POD_type pod_type;
	return _uninitialized_fill_n_aux(first, n, x, pod_type());
}
template<typename ForwardIter, typename Size, typename Tp>
inline ForwardIter
uninitialized_fill_n(ForwardIter first, Size n, const Tp& x) {
	return _uninitialized_fill_n(first, n, x, VALUE_TYPE(first));
}


// copy and fill
template<typename InputIter1, typename InputIter2, typename ForwardIter>
inline ForwardIter _uninitialized_copy_copy(InputIter1 first1, InputIter1 last1,
	                                        InputIter2 first2, InputIter2 last2,
	                                        ForwardIter result){
	ForwardIter mid = uninitialized_copy(first1, last1, result);
	return uninitialized_copy(first2, last2, mid);
}

template<typename ForwardIter, typename Tp, typename InputIter>
inline ForwardIter
_uninitialized_fill_copy(ForwardIter result, ForwardIter mid,
	                     const Tp&x, InputIter first, InputIter last) {
	uninitialized_fill(result, mid, x);
	return uninitialized_copy(first, last, mid);
}

template<typename ForwardIter, typename Tp, typename InputIter>
inline ForwardIter
_uninitialized_copy_fill(InputIter first1, InputIter last1,
	                     ForwardIter first2, ForwardIter last2,
	                     const Tp& x) {
	ForwardIter mid = uninitialized_copy(first1, last1, first2);
	return uninitialized_fill(mid, last2, x);
}

NAMESPACE_END

#endif // STL_UNINITIALIZED_H