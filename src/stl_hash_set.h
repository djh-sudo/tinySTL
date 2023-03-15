#ifndef STL_HASH_SET_H
#define STL_HASH_SET_H

#include "stl_config.h"
#include "stl_function.h"
#include "stl_hashtable.h"

NAMESPACE_START

template<typename Val, typename HashFunc=hash<Val>,
	     typename EqualKey=equal_to<Val>,typename Alloc=alloc>
class hash_set {
private:
	typedef hashtable<Val, Val, HashFunc, Identity<Val>, EqualKey, Alloc>Ht;
	Ht m_ht;

public:
	typedef typename Ht::key_type key_type;
	typedef typename Ht::value_type value_type;
	typedef typename Ht::hasher hasher;
	typedef typename Ht::key_equal key_equal;
	typedef typename Ht::size_type size_type;
	typedef typename Ht::difference_type difference_type;
	typedef typename Ht::const_pointer const_pointer;
	typedef typename Ht::const_pointer pointer;
	typedef typename Ht::const_reference reference;
	typedef typename Ht::const_reference const_reference;
	typedef typename Ht::const_iterator iterator;
	typedef typename Ht::const_iterator const_iterator;
	typedef typename Ht::allocator_type allocator_type;

	hasher hash_func()const { return m_ht.hash_fun(); }
	key_equal key_eq()const { return m_ht.key_eq(); }
	allocator_type get_allocator()const { return m_ht.get_allocator(); }
	friend bool operator==(const hash_set&, const hash_set&);
	friend bool operator!=(const hash_set&, const hash_set&);

public:
	hash_set() :m_ht(100, hasher(), key_equal(), allocator_type()) {}
	explicit hash_set(size_type n) :m_ht(n, hasher(), key_equal(), allocator_type()) {}
	hash_set(size_type n, const hasher& hf) :m_ht(n, hf, key_equal(), allocator_type()) {}
	hash_set(size_type n, const hasher& hf, key_equal&eq) :m_ht(n, hf, eq, allocator_type()) {}
	// other construction
	hash_set(const value_type* first, const value_type* last):m_ht(100,hasher(), key_equal(),allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_set(const value_type* first, const value_type* last, size_type n) 
		:m_ht(n, hasher(), key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_set(const value_type* first, const value_type* last, size_type n, const hasher&hf)
		:m_ht(n, hf, key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_set(const value_type* first, const value_type* last, size_type n, const hasher& hf, key_equal&eq)
		:m_ht(n, hf, eq, allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	// iterator initialize
	hash_set(const_iterator first, const_iterator last) :m_ht(100, hasher(), key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_set(const_iterator first, const_iterator last, size_type n)
		:m_ht(n, hasher(), key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_set(const_iterator first, const_iterator last, size_type n, const hasher& hf)
		:m_ht(n, hf, key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_set(const_iterator first, const_iterator last, size_type n, const hasher& hf, key_equal& eq)
		:m_ht(n, hf, eq, allocator_type()) {
		m_ht.insert_unique(first, last);
	}
public:
	inline size_type size()const { return m_ht.size(); }
	inline size_type max_size()const { return m_ht.max_size(); }
	inline bool empty()const { return m_ht.empty(); }
	inline void swap(hash_set& h) { m_ht.swap(h.m_ht); }

	iterator begin()const { return m_ht.begin(); }
	iterator end()const { return m_ht.end(); }
public:
	pair<iterator, bool>insert(const value_type& obj) {
		pair<typename Ht::iterator, bool>p = m_ht.insert_unique(obj);
		return pair<iterator, bool>(p.first, p.second);}

	void insert(const value_type* first, const value_type* last) {
		m_ht.insert_unique(first, last);}

	void insert(const_iterator first, const_iterator last) {
		m_ht.insert_unique(first, last);}

	pair<iterator, bool>insert_noresize(const value_type& obj) {
		pair<typename Ht::iterator, bool>p = m_ht.insert_unique_noresize(obj);
		return pair<iterator, bool>(p.first, p.second);
	}
	
	iterator find(const key_type& key)const {
		return m_ht.find(key);}
	size_type count(const key_type& key)const {
		return m_ht.count(key);}
	pair<iterator, iterator>equal_range(const key_type& key)const { return m_ht.equal_range(key); }
	
	size_type erase(const key_type& key) { return m_ht.erase(key); }
	void erase(iterator it) { m_ht.erase(it); }
	void erase(iterator first, iterator last) { m_ht.erase(first, last); }
	void clear() { m_ht.clear(); }

	void resize(size_type hint) { m_ht.resize(hint); }

	size_type bucket_count()const { return m_ht.bucket_count(); }
	size_type max_bucket_count()const { return m_ht.max_bucket_count(); }
	size_type ele_in_bucket(size_type n)const { return m_ht.ele_in_bucket(n); }
};
// overload operator
template<typename Val, typename HashFunc, typename EqualKey, typename Alloc>
bool operator==(const hash_set<Val, HashFunc, EqualKey, Alloc>&x,
	            const hash_set<Val, HashFunc, EqualKey, Alloc>&y){
	return x.m_ht == y.m_ht;}

template<typename Val, typename HashFunc, typename EqualKey, typename Alloc>
bool operator!=(const hash_set<Val, HashFunc, EqualKey, Alloc>& x,
	const hash_set<Val, HashFunc, EqualKey, Alloc>& y) {
	return !(x.m_ht == y.m_ht);
}

// multi-set
template<typename Val, typename HashFunc = hash<Val>,
	typename EqualKey = equal_to<Val>, typename Alloc = alloc>
class hash_multiset {
private:
	typedef hashtable<Val, Val, HashFunc, Identity<Val>, EqualKey, Alloc>Ht;
	Ht m_ht;

public:
	typedef typename Ht::key_type key_type;
	typedef typename Ht::value_type value_type;
	typedef typename Ht::hasher hasher;
	typedef typename Ht::key_equal key_equal;
	typedef typename Ht::size_type size_type;
	typedef typename Ht::difference_type difference_type;
	typedef typename Ht::const_pointer const_pointer;
	typedef typename Ht::const_pointer pointer;
	typedef typename Ht::const_reference reference;
	typedef typename Ht::const_reference const_reference;
	typedef typename Ht::const_iterator iterator;
	typedef typename Ht::const_iterator const_iterator;
	typedef typename Ht::allocator_type allocator_type;

	hasher hash_func()const { return m_ht.hash_fun(); }
	key_equal key_eq()const { return m_ht.key_eq(); }
	allocator_type get_allocator()const { return m_ht.get_allocator(); }
	friend bool operator==(const hash_multiset&, const hash_multiset&);
	friend bool operator!=(const hash_multiset&, const hash_multiset&);

public:
	hash_multiset() :m_ht(100, hasher(), key_equal(), allocator_type()) {}
	explicit hash_multiset(size_type n) :m_ht(n, hasher(), key_equal(), allocator_type()) {}
	hash_multiset(size_type n, const hasher& hf) :m_ht(n, hf, key_equal(), allocator_type()) {}
	hash_multiset(size_type n, const hasher& hf, key_equal& eq) :m_ht(n, hf, eq, allocator_type()) {}
	// other construction
	hash_multiset(const value_type* first, const value_type* last) :m_ht(100, hasher(), key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_multiset(const value_type* first, const value_type* last, size_type n)
		:m_ht(n, hasher(), key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_multiset(const value_type* first, const value_type* last, size_type n, const hasher& hf)
		:m_ht(n, hf, key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_multiset(const value_type* first, const value_type* last, size_type n, const hasher& hf, key_equal& eq)
		:m_ht(n, hf, eq, allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	// iterator initialize
	hash_multiset(const_iterator first, const_iterator last) :m_ht(100, hasher(), key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_multiset(const_iterator first, const_iterator last, size_type n)
		:m_ht(n, hasher(), key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_multiset(const_iterator first, const_iterator last, size_type n, const hasher& hf)
		:m_ht(n, hf, key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_multiset(const_iterator first, const_iterator last, size_type n, const hasher& hf, key_equal& eq)
		:m_ht(n, hf, eq, allocator_type()) {
		m_ht.insert_unique(first, last);
	}
public:
	inline size_type size()const { return m_ht.size(); }
	inline size_type max_size()const { return m_ht.max_size(); }
	inline bool empty()const { return m_ht.empty(); }
	inline void swap(hash_multiset& h) { m_ht.swap(h.m_ht); }

	iterator begin()const { return m_ht.begin(); }
	iterator end()const { return m_ht.end(); }
public:
	pair<iterator, bool>insert(const value_type& obj) {
		pair<typename Ht::iterator, bool>p = m_ht.insert_unique(obj);
		return pair<iterator, bool>(p.first, p.second);
	}

	void insert(const value_type* first, const value_type* last) {
		m_ht.insert_unique(first, last);
	}

	void insert(const_iterator first, const_iterator last) {
		m_ht.insert_unique(first, last);
	}

	pair<iterator, bool>insert_noresize(const value_type& obj) {
		pair<typename Ht::iterator, bool>p = m_ht.insert_unique_noresize(obj);
		return pair<iterator, bool>(p.first, p.second);
	}

	iterator find(const key_type& key)const {
		return m_ht.find(key);
	}
	size_type count(const key_type& key)const {
		return m_ht.count(key);
	}
	pair<iterator, iterator>equal_range(const key_type& key)const { return m_ht.equal_range(key); }

	size_type erase(const key_type& key) { return m_ht.erase(key); }
	void erase(iterator it) { m_ht.erase(it); }
	void erase(iterator first, iterator last) { m_ht.erase(first, last); }
	void clear() { m_ht.clear(); }

	void resize(size_type hint) { m_ht.resize(hint); }

	size_type bucket_count()const { return m_ht.bucket_count(); }
	size_type max_bucket_count()const { return m_ht.max_bucket_count(); }
	size_type ele_in_bucket(size_type n)const { return m_ht.ele_in_bucket(n); }
};
// overload operator
template<typename Val, typename HashFunc, typename EqualKey, typename Alloc>
bool operator==(const hash_multiset<Val, HashFunc, EqualKey, Alloc>& x,
	            const hash_set<Val, HashFunc, EqualKey, Alloc>& y) {
	return x.m_ht == y.m_ht;
}

template<typename Val, typename HashFunc, typename EqualKey, typename Alloc>
bool operator!=(const hash_multiset<Val, HashFunc, EqualKey, Alloc>& x,
	            const hash_set<Val, HashFunc, EqualKey, Alloc>& y) {
	return !(x.m_ht == y.m_ht);
}
NAMESPACE_END
#endif // STL_HASH_SET_H