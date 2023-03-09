#ifndef STL_STACK_H
#define STL_STACK_H

#include "stl_config.h"
#include "stl_queue.h"

NAMESPACE_START

template<typename Tp, typename Container=deque<Tp>>
class stack {
public:
	typedef typename Container::value_type      value_type;
	typedef typename Container::size_type       size_type;
	typedef Container                           container_type;
	typedef typename Container::reference       reference;
	typedef typename Container::const_reference const_reference;

public:
	stack() :c() {}
	explicit stack(stack&_):c(_){}
	inline bool empty()const { return c.empty(); }
	inline size_type size()const { return c.size(); }
	inline reference top() { return c.back(); }
	inline const_reference top()const { return c.back(); }
	inline void push(Tp& value) { c.push_back(value); }
	inline void pop() { c.pop_back(); }
	inline void swap(stack&x) { c.swap(x.c); }

protected:
	Container c;
};
// override operator
template<typename Tp, typename Container>
inline bool operator==(stack<Tp, Container>&x, stack<Tp, Container>&y){
	return x == y;}

template<typename Tp, typename Container>
inline bool operator<(stack<Tp, Container>& x, stack<Tp, Container>& y) {
	return x <= y;
}

NAMESPACE_END
#endif