#ifndef STL_ALGOBASE_H
#define STL_ALGOBASE_H

#include "string.h"
#include "stl_config.h"
#include "type_traits.h"
#include "stl_iterator.h"
#include "stl_pair.h"

NAMESPACE_START

template<typename ForwardIter1, typename ForwardIter2, typename Tp>
inline void _iter_swap(ForwardIter1 a, ForwardIter1 b, Tp*) {
	Tp tmp = *a;
	*a = *b;
	*b = tmp;
}

template<typename ForwardIter1, typename ForwardIter2>
inline void iter_swap(ForwardIter1 a, ForwardIter1 b) {
	_iter_swap(a, b, VALUE_TYPE(a));
}

template<typename Tp>
inline void swap(Tp& a, Tp& b) {
	Tp tmp = a;
	a = b;
	b = tmp;
}

template<typename Tp>
inline const Tp& min(const Tp& a, const Tp& b) { return b < a ? b : a; }

template<typename Tp>
inline const Tp& max(const Tp& a, const Tp& b) { return a < b ? b : a; }

template<typename Tp, typename Compare>
inline const Tp& min(const Tp& a, const Tp& b, Compare c) {
	return c(b, a) ? b : a; }

template<typename Tp, typename Compare>
inline const Tp& max(const Tp& a, const Tp& b, Compare c) {
	return c(a, b) ? b : a;
}

// copy
template<typename InputIter, typename OutputIter, typename Distance>
inline OutputIter _copy(InputIter first, InputIter last,
	                    OutputIter result,
	                    input_iterator_tag, Distance*){
	for (; first != last; ++first, ++result) {
		*result = *first;
	}
	return result;
}

template<typename RandomIter, typename OutputIter, typename Distance>
inline OutputIter _copy(RandomIter first, RandomIter last,
	                    OutputIter result,
	                    random_access_iterator_tag, Distance*) {
	for (; first != last; ++first, ++result) {
		*result = *first;
	}
	return result;
}

template<typename Tp>
inline Tp* _copy_trivial(const Tp* first, const Tp* last, Tp* result) {
	::memmove(result, first, sizeof(Tp) * (last - first));
	return result + (last - first);
}

template<typename InputIter, typename OutputIter>
inline OutputIter _copy_aux2(InputIter first, InputIter last, OutputIter result, _false_type) {
	return _copy(first, last, result, 
		         ITERATOR_CATEGORY(first),
		         DISTANCE_TYPE(first));
}

template<typename InputIter, typename OutputIter>
inline OutputIter _copy_aux2(InputIter first, InputIter last, OutputIter result, _true_type) {
	return _copy(first, last, result,
	             ITERATOR_CATEGORY(first),
	             DISTANCE_TYPE(first));
}

template<typename Tp>
inline Tp* _copy_aux2(Tp* first, Tp* last, Tp* result, _true_type) {
	return _copy_trivial(first, last, result);
}

template<typename Tp>
inline Tp* _copy_aux2(const Tp* first, const Tp* last, Tp* result, _true_type) {
	return _copy_trivial(first, last, result);
}

template<typename InputIter,typename OutputIter, typename Tp>
inline OutputIter _copy_aux(InputIter first, InputIter last, OutputIter result, Tp*) {
	typedef typename _type_traits<Tp>::has_trivial_assignment_operator trivial;
	return _copy_aux2(first, last, result, trivial());
}

template<typename InputIter, typename OutputIter>
inline OutputIter copy(InputIter first, InputIter last, OutputIter result) {
	return _copy_aux(first, last, result, VALUE_TYPE(first));
}
// copy backward
template<typename BidIter1, typename BidIter2, typename Distance>
inline BidIter2 _copy_backward(BidIter1 first, BidIter1 last, BidIter2 result,
	                           bidirectional_iterator_tag, Distance*) {
	while (first != last) {
		*--result = *--last;
		return result;
	}
}

template<typename Tp, typename Distance>
inline Tp* _copy_backward(Tp* first, Tp* last, Tp* result,
	                      random_access_iterator_tag, Distance*) {
	const ptrdiff_t num = last - first;
	::memmove(result - num, first, num * sizeof(Tp));
	return result - num;
}

template<typename Tp, typename Distance>
inline Tp* _copy_backward(const Tp* first, const Tp* last, Tp* result,
	                      random_access_iterator_tag, Distance*) {
	const ptrdiff_t num = last - first;
	::memmove(result - num, first, num * sizeof(Tp));
	return result - num;
}

template<typename RandAccIter, typename BidIter, typename Distance>
inline BidIter _copy_backward(RandAccIter first, RandAccIter last, BidIter result,
	                          random_access_iterator_tag, Distance*) {
	for (Distance n = last - first; n > 0; --n) {
		*--result = *--last;
	}
	return result;
}

template<typename BIter1, typename BIter2>
inline BIter2 copy_backward(BIter1 first, BIter1 last, BIter2 result) {
	return _copy_backward(first, last, result,
		                  ITERATOR_CATEGORY(first),
		                  DISTANCE_TYPE(first));
}

template<typename InputIter, typename Size, typename OutputIter>
inline pair<InputIter, OutputIter>_copy_n(InputIter first, Size count, 
	                               OutputIter result, input_iterator_tag) {
	for (; count > 0; --count) {
		*result = *first;
		++first;
		++result;
	}
	return pair<InputIter, OutputIter>(first, result);
}

template<typename RAIter, typename Size, typename OutputIter>
inline pair<RAIter, OutputIter>_copy_n(RAIter first, Size count,
	                                   OutputIter result, random_access_iterator_tag) {
	RAIter last = first + count;
	return pair<RAIter, OutputIter>(last, copy(first, last, result));
}

template<typename InputIter, typename Size, typename OutputIter>
inline pair<InputIter, OutputIter>_copy_n(InputIter first, Size count, OutputIter result) {
	return _copy_n(first, count, result, ITERATOR_CATEGORY(first));
}

template<typename InputIter, typename Size, typename OutputIter>
inline pair<InputIter, OutputIter>copy_n(InputIter first, Size count, OutputIter result) {
	return _copy_n(first, count, result);
}
// fill
template<typename ForwardIter,typename Tp>
void fill(ForwardIter first, ForwardIter last, Tp& x) {
	for (; first != last; ++first) {
		*first = x;
	}
}
template<typename OutputIter, typename Size, typename Tp>
OutputIter fill_n(OutputIter first, Size n, Tp& x) {
	for (; n > 0; --n, ++first) {
		*first = x;
	}
	return first;
}
// specialization
inline void fill(unsigned char* first, unsigned char* last, const unsigned char c) {
	::memset(first, c, (size_t)(last - first));
}

inline void fill(char* first, char* last, const char c) {
	::memset(first, (unsigned char)c, (size_t)(last - first));
}

template<typename Size>
inline unsigned char* fill_n(unsigned char* first, Size n, const unsigned char c) {
	fill(first, first + n, c);
	return first + n;
}

template<typename Size>
inline char* fill_n(char* first, Size n, const char c) {
	fill(first, first + n, c);
	return first + n;
}
// equal/mismatch
template<typename InputIter1, typename InputIter2>
pair< InputIter1, InputIter2>mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
	while (first1 != last1 && *first1 == *first2) {
		++first1;
		++first2;
	}
	return pair< InputIter1, InputIter2>(first1, first2);
}
template<typename InputIter1, typename InputIter2, typename BinaryPre>
pair< InputIter1, InputIter2>mismatch(InputIter1 first1, InputIter1 last1,
	                                  InputIter2 first2, BinaryPre predicate) {
	while (first1 != last1 && predicate(*first1, *first2)) {
		++first1;
		++first2;
	}
	return pair< InputIter1, InputIter2>(first1, first2);
}
template<typename InputIter1, typename InputIter2>
inline bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
	while (first1 != last1) {
		if (*first1 != *first2) return false;
		++first1;
		++first2;
	}
	return true;
}
template<typename InputIter1, typename InputIter2, typename BinaryPre>
inline bool equal(InputIter1 first1, InputIter1 last1, 
	              InputIter2 first2, BinaryPre predicate) {
	for (; first1 != last1; ++first1, ++first2) {
		if (!predicate(*first1, *first2))return false;
	}
	return true;
}
template<typename InputIter1, typename InputIter2>
inline bool lexicalgraphical_compare(InputIter1 first1, InputIter1 last1,
	                                 InputIter2 first2, InputIter2 last2){
	for (; first1 != last1 && first2 != last2;++first1,++first2) {
		if (*first1 < *first2)return true;
		if (*first2 < *first1)return false;
	}
	return first1 == last1 && first2 != last2;
}
template<typename InputIter1, typename InputIter2, typename Compare>
inline bool lexicalgraphical_compare(InputIter1 first1, InputIter1 last1,
	                                 InputIter2 first2, InputIter2 last2,
	                                 Compare compare) {
	for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
		if (compare(*first1, *first2))return true;
		if (compare(*first2, *first1))return false;
	}
	return first1 == last1 && first2 != last2;
}

inline bool lexigraphical_compare(const unsigned char* first1,
	                              const unsigned char* last1,
	                              const unsigned char* first2,
	                              const unsigned char* last2){
	const size_t len1 = size_t(last1 - first1);
	const size_t len2 = size_t(last2 - first2);
	const int result = ::memcmp(first1, first2, min(len1, len2));
	return result != 0 ? result < 0 : len1 < len2;
}
inline bool lexigraphical_compare(const char* first1, const char* last1,
	                              const char* first2, const char* last2){
	return lexigraphical_compare((const unsigned char*)first1,
	                             (const unsigned char*)last1,
	                             (const unsigned char*)first2,
	                             (const unsigned char*)last2);
}
// 3 way

NAMESPACE_END

#endif // STL_ALGOBASE_H