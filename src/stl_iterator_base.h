#ifndef STL_ITERATOR_BASE_H
#define STL_ITERATOR_BASE_H

#include "stl_config.h"

NAMESPACE_START

// iterator tag
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag :public input_iterator_tag{};
struct bidirectional_iterator_tag :public forward_iterator_tag {};
struct random_access_iterator_tag:public bidirectional_iterator_tag {};

// iterator
template<typename Tp, typename Distance>struct input_iterator {
	typedef input_iterator_tag  iterator_category;
	typedef Tp                  value_type;
	typedef Distance            different_type;
	typedef Tp*                 pointer;
	typedef Tp&                 reference;
};

template<typename Tp, typename Distance>struct output_iterator {
	typedef output_iterator_tag  iterator_category;
	typedef void                 value_type;
	typedef void                 different_type;
	typedef void                 pointer;
	typedef void                 reference;
};

template<typename Tp, typename Distance>struct forward_iterator {
	typedef forward_iterator_tag  iterator_category;
	typedef Tp                    value_type;
	typedef Distance              different_type;
	typedef Tp*                   pointer;
	typedef Tp&                   reference;
};

template<typename Tp, typename Distance>struct bidirectional_iterator {
	typedef bidirectional_iterator_tag  iterator_category;
	typedef Tp                          value_type;
	typedef Distance                    different_type;
	typedef Tp*                         pointer;
	typedef Tp&                         reference;
};

template<typename Tp, typename Distance>struct random_access_iterator {
	typedef random_access_iterator_tag  iterator_category;
	typedef Tp                          value_type;
	typedef Distance                    different_type;
	typedef Tp*                         pointer;
	typedef Tp&                         reference;
};

// category
template<typename Tp, typename Distance>
inline input_iterator_tag
iterator_category(const input_iterator<Tp, Distance>&) {
	return input_iterator_tag();
}

template<typename Tp, typename Distance>
inline output_iterator_tag
iterator_category(const output_iterator<Tp, Distance>&) {
	return output_iterator_tag();
}

template<typename Tp, typename Distance>
inline forward_iterator_tag
iterator_category(const forward_iterator<Tp, Distance>&) {
	return forward_iterator_tag();
}

template<typename Tp, typename Distance>
inline bidirectional_iterator_tag
iterator_category(const bidirectional_iterator<Tp, Distance>&) {
	return bidirectional_iterator_tag();
}

template<typename Tp, typename Distance>
inline random_access_iterator_tag
iterator_category(const random_access_iterator<Tp, Distance>&) {
	return random_access_iterator_tag();
}

template<typename Tp>
inline random_access_iterator_tag
iterator_category(const Tp*) {
	return random_access_iterator_tag();
}

// value type
template<typename Tp, typename Distance>
inline Tp* value_type(const input_iterator<Tp, Distance>&) {
	return (Tp*)(0);
}

template<typename Tp, typename Distance>
inline Tp* value_type(const forward_iterator<Tp, Distance>&) {
	return (Tp*)(0);
}

template<typename Tp, typename Distance>
inline Tp* value_type(const bidirectional_iterator<Tp, Distance>&) {
	return (Tp*)(0);
}

template<typename Tp, typename Distance>
inline Tp* value_type(const random_access_iterator<Tp, Distance>&) {
	return (Tp*)(0);
}

template<typename Tp>
inline Tp* value_type(const Tp*) { return (Tp*)(0); }

// distance type
template<typename Tp, typename Distance>
inline Distance* distance_type(const input_iterator<Tp, Distance>&) {
	return (Distance*)(0);
}

template<typename Tp, typename Distance>
inline Distance* distance_type(const forward_iterator<Tp, Distance>&) {
	return (Distance*)(0);
}

template<typename Tp, typename Distance>
inline Distance* distance_type(const bidirectional_iterator<Tp, Distance>&) {
	return (Distance*)(0);
}

template<typename Tp, typename Distance>
inline Distance* distance_type(const random_access_iterator<Tp, Distance>&) {
	return (Distance*)(0);
}

template<typename Tp>
inline ptrdiff_t* distance_type(const Tp*) {
	return (ptrdiff_t*)(0);
}
// macro define
#define ITERATOR_CATEGORY(X) iterator_category(X)
#define DISTANCE_TYPE(X) distance_type(X)
#define VALUE_TYPE(X) value_type(X)

// distance 
template<typename InputIterator, typename Distance>
inline void distance(InputIterator first, InputIterator last, Distance& n, input_iterator_tag) {
	while (first != last) {
		++first;
		++n;
	} // O(n)
}

template<typename RandomAccessIterator, typename Distance>
inline void distance(RandomAccessIterator first, 
	                 RandomAccessIterator last,
	                 Distance& n,
	                 random_access_iterator_tag) {
	n += (last - first); // O(1)
}

template<typename InputIterator, typename Distance>
inline void distance(InputIterator first, InputIterator last, Distance& n) {
	return distance(first, last, n, iterator_category(first));
	// other type O(n)
}

// advance (how to go ahead)
template<typename InputIterator, typename Distance>
inline void advance(InputIterator& i, Distance n, input_iterator_tag) {
	while (n--)++i;
}

template<typename BidirectionalInterator, typename Distance>
inline void advance(BidirectionalInterator& i, Distance n, bidirectional_iterator_tag) {
	if (n >= 0) {
		while (n--)++i;
	}
	else {
		while (n++)--i;
	}
}

template<typename RandomAccessIterator, typename Distance>
inline void advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag) {
	i += n;
}

template<typename InputInterator, typename Distance>
inline void advance(InputInterator& i, Distance n) {
	advance(i, n, iterator_category(i));
}

NAMESPACE_END
#endif // STL_ITERATOR_BASE_H
