#ifndef STL_ITERATOR_H
#define STL_ITERATOR_H

#include <stddef.h>
#include "stl_config.h"
#include "stl_iterator_base.h"


NAMESPACE_START
// back_insert_iterator
template<typename Container>
class BackInsertIter {
public:
	typedef Container            container_type;
	typedef output_iterator_tag  iterator_category;
	typedef void                 value_type;
	typedef void                 difference_type;
	typedef void                 ppinter;
	typedef void                 reference;

	explicit BackInsertIter(Container& x) :container(x) {}

	BackInsertIter<Container>& operator=(const typename Container::value_type& value) {
		container->push_back(value);
		return *this;
	}
	
	BackInsertIter<Container>& operator*() { return *this; }
	// no meaning
	BackInsertIter<Container>& operator++() { return *this; }
	BackInsertIter<Container>& operator++(int) { return *this; }

protected:
	Container* container;
};

template<typename Container>
inline output_iterator_tag
iterator_category(const BackInsertIter<Container>&) {
	return output_iterator_tag();
}

template<typename Container>
inline BackInsertIter<Container>back_inserter(Container& x) {
	return BackInsertIter<Container>(x);
}
// front_insert_iterator
template<typename Container>
class FrontInsertIter {

public:
	typedef Container            container_type;
	typedef output_iterator_tag  iterator_category;
	typedef void                 value_type;
	typedef void                 difference_type;
	typedef void                 ppinter;
	typedef void                 reference;

	explicit FrontInsertIter(Container&x):container(x) {}
	FrontInsertIter<Container>& operator=(const typename Container::value_type& value) {
		container->push_front(value);
		return*this;
	}
	FrontInsertIter<Container>& operator*() { return *this; }
	// no meaning
	FrontInsertIter<Container>& operator++() { return *this; }
	FrontInsertIter<Container>& operator++(int) { return *this; }

protected:
	Container* container;
};

template<typename Container>
inline output_iterator_tag
iterator_category(const FrontInsertIter<Container>&) {
	return output_iterator_tag();
}

template<typename Container>
inline FrontInsertIter<Container>front_inserter(Container& x) {
	return FrontInsertIter<Container>(x);
}
// insert iterator
template<typename Container>
class InsertIter {

public:
	typedef Container            container_type;
	typedef output_iterator_tag  iterator_category;
	typedef void                 value_type;
	typedef void                 difference_type;
	typedef void                 ppinter;
	typedef void                 reference;

	InsertIter(Container& x, typename Container::iterator i):container(x), iter(i) {}
	InsertIter<Container>& operator=(const typename Container::value_type& value) {
		iter = container->insert(iter, value);
		++iter;
		return *this;
	}
	InsertIter<Container>& operator*() { return *this; }
	// no meaning
	InsertIter<Container>& operator++() { return *this; }
	InsertIter<Container>& operator++(int) { return *this; }

protected:
	Container* container;
	typename Container::iterator iter;
};

template<typename Container>
inline output_iterator_tag
iterator_category(const InsertIter<Container>&) {
	return output_iterator_tag();
}

template<typename Container, typename Interator>
inline InsertIter<Container>inserter(Container& x, Interator& i) {
	typename Container::iterator it;
	return InsertIter<Container>(x, it);
}
// reverse bidirectional iterator
template<typename BidirectionalIter, typename Tp, 
         typename Ref = Tp&, typename Distance=ptrdiff_t>
class ReverseBidirectionalIter {

	typedef ReverseBidirectionalIter< BidirectionalIter, Tp, Ref, Distance> Self;

public:
	typedef bidirectional_iterator_tag iterator_category;
	typedef Tp                         value_type;
	typedef Distance                   difference_type;
	typedef Tp*                        pointer;
	typedef Ref                        reference;
	
	ReverseBidirectionalIter() = default;
	
	explicit ReverseBidirectionalIter(BidirectionalIter x) :current(x) {}
	
	BidirectionalIter base()const { return current; }

	Ref operator*()const {
		BidirectionalIter tmp = current;
		return *--tmp;
	}

	pointer operator->()const { return &(operator*()); }
	Self& operator++() {
		--current;
		return *this;
	}
	Self operator++(int) {
		Self tmp = *this;
		--current;
		return tmp;
	}
	Self& operator--() {
		++current;
		return *this;
	}
	Self operator--(int) {
		Self tmp = *this;
		++current;
		return tmp;
	}
protected:
	BidirectionalIter current;
};

template<typename BidirectionalIter, typename Tp,
         typename Ref, typename Distance>
inline bidirectional_iterator_tag
iterator_category(const ReverseBidirectionalIter<BidirectionalIter, Tp, Ref, Distance>&) {
	return bidirectional_iterator_tag();
}

template<typename BidirectionalIter, typename Tp,
         typename Ref, typename Distance>
inline Tp*
value_type(const ReverseBidirectionalIter<BidirectionalIter, Tp, Ref, Distance>&) {
	return (Tp*)0;
}

template<typename BidirectionalIter, typename Tp,
	typename Ref, typename Distance>
inline Distance*
distance_type(const ReverseBidirectionalIter<BidirectionalIter, Tp, Ref, Distance>&) {
	return (Distance*)0;
}

template<typename BiIter, typename Tp, typename Ref, typename Distance>
inline bool operator==(
	const ReverseBidirectionalIter<BiIter, Tp, Ref, Distance>&x,
	const ReverseBidirectionalIter<BiIter, Tp, Ref, Distance>&y
	) {return x.base() == y.base(); }

template<typename BiIter, typename Tp, typename Ref, typename Distance>
inline bool operator!=(
	const ReverseBidirectionalIter<BiIter, Tp, Ref, Distance>& x,
	const ReverseBidirectionalIter<BiIter, Tp, Ref, Distance>& y
	) { // ???
	return x != y;
}

// reverse random access iterator
template<typename RandomAccessIter, typename Tp, typename Ref=Tp&, typename Distance=ptrdiff_t>
class ReverseIter {
	
	typedef ReverseIter<RandomAccessIter, Tp, Ref, Distance>Self;
public:
	typedef random_access_iterator_tag iterator_category;
	typedef Tp                         value_type;
	typedef Distance                   diffrence_type;
	typedef Tp*                        pointer;
	typedef Ref                        reference;

	ReverseIter() = default;
	explicit ReverseIter(RandomAccessIter x) :current(x) {}
	ReverseIter base() const { return current; }
	Ref operator*()const { return *(current - 1); }
	pointer operator->()const { return &(operator*()); }
	Self& operator++() {
		--current;
		return*this;
	}
	Self operator++(int) {
		Self tmp = *this;
		--current;
		return tmp;
	}
	Self& operator--() {
		++current;
		return *this;
	}
	Self operator--(int) {
		Self tmp = *this;
		++current;
		return tmp;
	}
	Self operator+(Distance n)const { return Self(current - n); }
	Self operator+=(Distance n) {
		current -= n;
		return *this;
	}
	Self operator-(Distance n)const { return Self(current + n); }
	Self& operator-=(Distance n) {
		current += n;
		return *this;
	}
	Ref operator[](Distance n)const { return *(*this + n); }

protected:
	RandomAccessIter current;
};

template<typename RandomAccessIter, typename Tp, typename Ref, typename Distance>
inline random_access_iterator_tag
iterator_category(const ReverseIter<RandomAccessIter, Tp, Ref, Distance>&) {
	return random_access_iterator_tag();}

template<typename RandomAccessIter, typename Tp, typename Ref, typename Distance>
inline Tp* value_type(const ReverseIter<RandomAccessIter, Tp, Ref, Distance>&) {
	return (Tp*)0;}

template<typename RandomAccessIter, typename Tp, typename Ref, typename Distance>
inline Distance* distance_type(const ReverseIter<RandomAccessIter, Tp, Ref, Distance>&) {
	return (Distance*)0;}

template<typename RandomAccessIter, typename Tp, typename Ref, typename Distance>
inline bool operator==(const ReverseIter<RandomAccessIter,Tp, Ref,Distance>&x,
	                   const ReverseIter<RandomAccessIter, Tp, Ref, Distance>& y){
	return x.base() == y.base();
}

template<typename RandomAccessIter, typename Tp, typename Ref, typename Distance>
inline bool operator<(const ReverseIter<RandomAccessIter,Tp, Ref,Distance>&x,
	                   const ReverseIter<RandomAccessIter, Tp, Ref, Distance>& y){
	return y.base() < x.base();
}

template<typename RandomAccessIter, typename Tp, typename Ref, typename Distance>
inline Distance operator-(const ReverseIter<RandomAccessIter,Tp, Ref,Distance>&x,
	                   const ReverseIter<RandomAccessIter, Tp, Ref, Distance>& y){
	return y.base() - x.base();
}

template<typename RandomAccessIter, typename Tp, typename Ref, typename Distance>
inline ReverseIter<RandomAccessIter,Tp,Ref,Distance>
operator+(Distance n, const ReverseIter<RandomAccessIter, Tp, Ref, Distance>& x) {
	return ReverseIter<RandomAccessIter, Tp, Ref, Distance>(x.base() - n);
}

// input/output iterator
NAMESPACE_END


#endif // 