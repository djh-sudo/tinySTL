#ifndef STL_HEAP_H
#define STL_HEAP_H

#include "stl_config.h"
#include "stl_iterator.h"

NAMESPACE_START
template<typename RACIter, typename Distance,typename Tp>
void _push_heap(RACIter first, Distance hole_index, Distance topIndex, const Tp& value) {
	Distance parent = (hole_index - 1) / 2;
	while (hole_index > topIndex && *(first + parent) < value) {
		*(first + hole_index) = *(first + parent);
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	}
	*(first + hole_index) = value;
}

template<typename RACIter, typename Distance, typename Tp, typename Compare>
void _push_heap(RACIter first, Distance holeIndex, Distance topIndex, const Tp &value, const Compare& cmp) {
	Distance parent = (holeIndex - 1) / 2;
	while (holeIndex > topIndex && cmp(*(first + parent), value)) {
		*(first + holeIndex) = *(first + parent);
		holeIndex = parent;
		parent = (holeIndex - 1) / 2;
	}
	*(first + holeIndex) = value;
}

template<typename RACIter, typename Distance, typename Tp>
inline void _push_heap_aux(RACIter first, RACIter last, Distance*, Tp*) {
	_push_heap(first, Distance(last - first - 1), Distance(0), Tp(*(last - 1))); }

template<typename RACIter, typename Distance, typename Tp, typename Compare>
inline void _push_heap_aux(RACIter first, RACIter last, Distance*, const Compare&cmp, Tp*) {
	_push_heap(first, Distance(last - first - 1), Distance(0), Tp(*(last - 1)), cmp);
}

template<typename RACIter>
inline void push_heap(RACIter first, RACIter last) {
	_push_heap_aux(first, last, DISTANCE_TYPE(first), VALUE_TYPE(first)); }

template<typename RACIter, typename Compare>
inline void push_heap(RACIter first, RACIter last, const Compare&cmp) {
	_push_heap_aux(first, last, DISTANCE_TYPE(first), cmp, VALUE_TYPE(first));
}

// adjust down
template<typename RACIter, typename Distance, typename Tp>
void _adjust_heap(RACIter first, Distance holeIndex, Distance len, const Tp& value) {
	// adjust down
	Distance topIndex = holeIndex;
	Distance secondChild = 2 * holeIndex + 2;
	while (secondChild < len) {
		if (*(first + secondChild) < *(first + secondChild - 1)) {
			--secondChild;
		}
		// find bigger child
		*(first + holeIndex) = *(first + secondChild);
		holeIndex = secondChild;
		secondChild = 2 * (secondChild + 1);
	}
	if (secondChild == len) {
		*(first + holeIndex) = *(first + (secondChild - 1));
		holeIndex = secondChild - 1;
	}
	_push_heap(first, holeIndex, topIndex, value);
}

template<typename RACIter, typename Distance, typename Tp, typename Compare>
void _adjust_heap(RACIter first, Distance holeIndex, Distance len, const Tp& value, const Compare& cmp) {
	// adjust down
	Distance topIndex = holeIndex;
	Distance secondChild = 2 * holeIndex + 2;
	while (secondChild < len) {
		if (cmp(*(first + secondChild), *(first + secondChild - 1))) {
			--secondChild;
		}
		// second child is smaller
		*(first + holeIndex) = *(first + secondChild);
		holeIndex = secondChild;
		secondChild = 2 * (secondChild + 1);
	}
	if (secondChild == len) {
		*(first + holeIndex) = *(first + (secondChild - 1));
		holeIndex = secondChild - 1;
	}
	_push_heap(first, holeIndex, topIndex, value, cmp);
}

template<typename RACIter, typename Distance, typename Tp>
inline void _pop_heap(RACIter first, RACIter last
	, RACIter result, const Tp& value, Distance*) {
	*result = *first;
	_adjust_heap(first, Distance(0), Distance(last - first), value);
}

template<typename RACIter, typename Distance, typename Tp, typename Compare>
inline void _pop_heap(RACIter first, RACIter last
	, RACIter result, const Tp& value, const Compare&cmp, Distance*) {
	*result = *first;
	_adjust_heap(first, Distance(0), Distance(last - first), value, cmp);
}

template<typename RACIter, typename Tp>
inline void pop_heap_aux(RACIter first, RACIter last, Tp*) {
	_pop_heap(first, last - 1, last - 1,
		      Tp(*(last - 1)), DISTANCE_TYPE(first)); }

template<typename RACIter, typename Tp, typename Compare>
inline void pop_heap_aux(RACIter first, RACIter last, Tp*, const Compare& cmp) {
	_pop_heap(first, last - 1, last - 1,
		      Tp(*(last - 1)), cmp, DISTANCE_TYPE(first)); }

template<typename RACIter>
inline void pop_heap(RACIter first, RACIter last) {
	pop_heap_aux(first, last, VALUE_TYPE(first));
}

template<typename RACIter, typename Compare>
inline void pop_heap(RACIter first, RACIter last, const Compare& cmp) {
	pop_heap_aux(first, last, VALUE_TYPE(first), cmp);
}

template<typename RACIter, typename Tp, typename Distance>
void _make_heap(RACIter first, RACIter last, Tp*, Distance*) {
	if (last - first < 2)return;
	Distance len = last - first;
	Distance parent = (len - 2) / 2;
	while (true) {
		_adjust_heap(first, parent, len, Tp(*(first + parent)));
		if (parent == 0) return;
		--parent;
	}
}

template<typename RACIter, typename Tp, typename Distance, typename Compare>
void _make_heap(RACIter first, RACIter last, const Compare cmp, Tp*, Distance*) {
	if (last - first < 2)return;
	Distance len = last - first;
	Distance parent = (len - 2) / 2;
	while (true) {
		_adjust_heap(first, parent, len, Tp(*(first + parent)), cmp);
		if (parent == 0) return;
		--parent;
	}
}

template<typename RACIter>
inline void make_heap(RACIter first, RACIter last) {
	_make_heap(first, last, VALUE_TYPE(first), DISTANCE_TYPE(first)); }

template<typename RACIter, typename Compare>
inline void make_heap(RACIter first, RACIter last, const Compare& cmp) {
	_make_heap(first, last, cmp, VALUE_TYPE(first), DISTANCE_TYPE(first)); }

// sort
template<typename RACIter>
void sort_heap(RACIter first, RACIter last) {
	while (last - first > 1)
		pop_heap(first, last--);
}

template<typename RACIter, typename Compare>
void sort_heap(RACIter first, RACIter last, const Compare& cmp) {
	while (last - first > 1)
		pop_heap(first, last--, cmp);
}

NAMESPACE_END
#endif // STL_HEAP_H