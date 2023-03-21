#ifndef STL_HASH_FUN_H
#define STL_HASH_FUN_H

#include "stl_config.h"
NAMESPACE_START
template <typename Key>
struct hash {};
size_t hash_string(const char* s){
	unsigned long res = 0;
	for (; *s != 0; ++s) {
		res = 5 * res + *s;
	}
	return (size_t)res;
}

template<>
struct hash<char*> { inline size_t operator()(const char* s)const { return hash_string(s); } };

template<>
struct hash<const char*> { inline size_t operator()(const char* s)const { return hash_string(s); } };

template<>
struct hash<char> { inline size_t operator()(char c)const { return (size_t)c; } };

template<>
struct hash<unsigned char> { inline size_t operator()(unsigned char c)const { return (size_t)c; } };

template<>
struct hash<int> { inline size_t operator()(int x)const { return (size_t)x; } };

template<>
struct hash<long> { inline size_t operator()(long x)const { return (size_t)x; } };

template<>
struct hash<size_t> { inline size_t operator()(size_t x)const { return (size_t)x; } };

NAMESPACE_END
#endif // STL_HASH_FUN_H