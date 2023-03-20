#ifndef STL_QUEUE_H
#define STL_QUEUE_H
#include "stl_config.h"
#include "stl_function.h"
#include "stl_heap.h"
#include "stl_deque.h"
#include "stl_vector.h"

NAMESPACE_START

template<typename Tp, typename Container=deque<Tp>>
class queue {
public:
	typedef typename Container::value_type      value_type;
	typedef typename Container::size_type       size_type;
	typedef Container                           container_type;
	typedef typename Container::reference       reference;
	typedef typename Container::const_reference const_reference;

public:
	queue():c() {}
	explicit queue(const Container& _):c(_) {}
	inline bool empty()const { return c.empty(); }
	inline size_type size()const { return c.size(); }
	inline reference front() { return c.front(); }
	inline const_reference front()const { return c.front(); }
	inline reference back() { return c.back(); }
	inline const_reference back()const { return c.back(); }
	inline void push(Tp& value) { c.push_back(value); }
	inline void pop() { c.pop_front(); }
	inline void swap(queue& x) { c.swap(x.c); }
	// delete
	reference operator[](size_type n) = delete;
	const_reference operator[](size_type n) const = delete;
	
protected:
	Container c;
};
// override
template<typename Tp, typename Container>
inline bool operator==(const Container& c1, const Container& c2) {
	return c1 == c2;}

template<typename Tp, typename Container>
inline bool operator<(const Container& c1, const Container& c2) {
	return c1 < c2;
}

// priority queue
template<typename Tp, typename Container=vector<Tp>,
	     typename Compare = less<typename Container::value_type>>
class priority_queue {
public:
	typedef typename Container::value_type value_type;
	typedef typename Container::size_type size_type;
	typedef Container container_type;
	typedef typename Container::reference reference;
	typedef typename Container::const_reference const_reference;

protected:
	Container c;
	Compare cmp;
public:
	priority_queue() :c() {}
	explicit priority_queue(const Compare& x) :c(), cmp(x) {}
	priority_queue(const Compare& x, const Container& s) :c(s), cmp(x) {
		make_heap(c.begin(), c.end(), cmp);}
	priority_queue(const value_type* first, const value_type* last):c(first, last) {
		make_heap(c.begin(), c.end(), cmp);}
	priority_queue(const value_type* first, const value_type* last, const Compare&x) :c(first, last), cmp(x) {
		make_heap(c.begin(), c.end(), cmp);
	}
	~priority_queue() = default;

	inline bool empty()const { return c.empty(); }
	
	inline size_type size()const { return c.size(); }
	
	inline const_reference top()const { return c.front(); }
	
	inline void push(const value_type& x) {
		c.push_back(x);
		push_heap(c.begin(), c.end(), cmp);
	}
	
	inline void pop() {
		pop_heap(c.begin(), c.end(), cmp);
		c.pop_back();
	}

	inline void clear() {c.clear();}
	// no iteration and dereference
	reference operator[](size_type n) = delete;

};
NAMESPACE_END
#endif // !STL_QUEUE_H