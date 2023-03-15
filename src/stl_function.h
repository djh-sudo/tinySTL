#ifndef STL_FUNCTION_H
#define STL_FUNCTION_H

#include "stl_config.h"

NAMESPACE_START

template<typename Arg, typename Res>
struct unary_function {
	typedef Arg argument;
	typedef Res result;
};

template<typename Arg1, typename Arg2,typename Res>
struct binary_function {
	typedef Arg1 first_argument;
	typedef Arg2 second_argument;
	typedef Res result;
};

template<typename Tp>
struct plus :public binary_function<Tp, Tp, Tp> {
	Tp operator()(const Tp& x, const Tp& y) const { return x + y; } };

template<typename Tp>
struct minus :public binary_function<Tp, Tp, Tp> {
	Tp operator()(const Tp& x, const Tp& y) const { return x - y; }
};

template<typename Tp>
struct multiplies :public binary_function<Tp, Tp, Tp> {
	Tp operator()(const Tp& x, const Tp& y) const { return x * y; }
};

template<typename Tp>
struct devides :public binary_function<Tp, Tp, Tp> {
	Tp operator()(const Tp& x, const Tp& y) const { return x / y; }
};
// other implementation ...
template<typename Tp>
struct equal_to :public binary_function<Tp, Tp, bool> {
	bool operator()(const Tp& x, const Tp& y)const { return x == y; }
};
 template<typename Pair>
 struct Select1st :public unary_function<Pair,typename Pair::first_type> {
	 const typename Pair::first_type& operator()(const Pair& x)const { return x.first; }
 };
NAMESPACE_END // STL_FUNCTION_H
#endif