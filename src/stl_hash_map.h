#ifndef H_HASH_MAP_H
#define H_HASH_MAP_H

#include "stl_config.h"
#include "stl_pair.h"
#include "stl_alloc.h"
#include "stl_hash_fun.h"
#include "stl_function.h"
#include "stl_hashtable.h"

NAMESPACE_START

template<typename Key, typename Tp, typename HashFun=hash<Key>,
	     typename EqualKey=equal_to<Key>,
	     typename Alloc=alloc>
class hash_map {

private:
	typedef hashtable<pair<const Key, Tp>, Key, HashFun, Select1st<pair<const Key, Tp>>, EqualKey, Alloc> Ht;
	Ht m_ht;

public:
	typedef typename Ht::key_type key_type;
	typedef Tp data_type;
	typedef Tp mapped_type;
	typedef typename Ht::value_type value_type;
	typedef typename Ht::hasher hasher;
	typedef typename Ht::key_equal key_equal;

	typedef typename Ht::size_type size_type;
	typedef typename Ht::difference_type difference_type;
	typedef typename Ht::pointer pointer;
	typedef typename Ht::const_pointer const_pointer;
	typedef typename Ht::reference reference;
	typedef typename Ht::const_reference const_reference;
	typedef typename Ht::iterator iterator;
	typedef typename Ht::const_iterator const_iterator;
	typedef typename Ht::allocator_type allocator_type;

	inline hasher hash_func()const { return m_ht.hash_fun(); }
	inline key_equal key_eq()const { return m_ht.key_eq(); }
	inline allocator_type get_allocator()const { return m_ht.get_allocator(); }

public:
	hash_map() :m_ht(100, hasher(), key_equal(), allocator_type()) {}
	explicit hash_map(size_type n) :m_ht(n, hasher(), key_equal(), allocator_type()) {}
	hash_map(size_type n, const hasher&hf):m_ht(n, hf, key_equal(), allocator_type()) {}
	hash_map(size_type n, const hasher& hf, const key_equal& eq) :m_ht(n, hf, eq, allocator_type()) {}
	// other construction
	hash_map(size_type* first, size_type* last) 
		:m_ht(100, hasher(), key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_map(size_type* first, size_type* last, size_type n)
		:m_ht(n, hasher(), key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_map(size_type* first, size_type* last, size_type n, hasher&hf)
		:m_ht(n, hf, key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_map(size_type* first, size_type* last, size_type n, hasher& hf, key_equal&eq)
		:m_ht(n, hf, eq, allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	// construct with iterator
	hash_map(const_iterator first, const_iterator last)
		:m_ht(100, hasher(), key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_map(const_iterator first, const_iterator last, size_type n)
		:m_ht(n, hasher(), key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_map(const_iterator first, const_iterator last, size_type n, hasher& hf)
		:m_ht(n, hf, key_equal(), allocator_type()) {
		m_ht.insert_unique(first, last);
	}
	hash_map(const_iterator first, const_iterator last, size_type n, hasher& hf, key_equal& eq)
		:m_ht(n, hf, eq, allocator_type()) {
		m_ht.insert_unique(first, last);
	}
public:
	inline size_type size()const { return m_ht.size(); }
	inline size_type max_size()const { return m_ht.max_size(); }
	inline bool empty()const { return m_ht.empty(); }
	inline void swap(hash_map& hp) { m_ht.swap(hp.m_ht); }
	// iterator
	inline iterator begin() { return m_ht.begin(); }
	inline iterator end() { return m_ht.end(); }
	inline const_iterator begin()const { return m_ht.begin(); }
	inline const_iterator end()const { return m_ht.end(); }
	//
	inline pair<iterator, bool>insert(const value_type& obj) { 
		return m_ht.insert_unique(obj); }
	inline void insert(const value_type* first, const value_type* last) {
		m_ht.insert_unique(first, last);}
	inline iterator find(const key_type& key) { return m_ht.find(key); }
	inline const_iterator find(const key_type& key)const { return m_ht.find(key); }
	inline Tp& operator[](const key_type& key) {
		return m_ht.find_or_insert(value_type(key, Tp())).second;}
	inline size_type count(const key_type& key)const { return m_ht.count(key); }

	inline pair<iterator, iterator>equal_range(const key_type& key) {
		return m_ht.equal_range(key);}
	inline pair<const_iterator, const_iterator>equal_range(const key_type& key) const{
		return m_ht.equal_range(key);}

	inline size_type erase(const key_type& key) { return m_ht.erase(key); }
	inline void erase(iterator it) { m_ht.erase(it); }
	inline void erase(iterator first, iterator last) { m_ht.erase(first, last); }

	inline void resize(size_type n) { m_ht.resize(n); }
	inline size_type bucket_count()const { return m_ht.bucket_count(); }
	inline size_type max_bucket_count()const { return m_ht.max_bucket_count(); }
	inline size_type ele_in_bucket(size_type n)const { return m_ht.ele_in_bucket(); }
};
// override operator
template<typename Key, typename Tp, typename HashFun, typename EqualKey, typename Alloc>
inline bool operator==(const hash_map<Key, Tp, HashFun, EqualKey, Alloc>&x,
	                   const hash_map<Key, Tp, HashFun, EqualKey, Alloc>&y){
	return x.m_ht == y.m_ht;
}

// multimap
template<typename Key, typename Tp, typename HashFun=hash<Key>, 
	     typename EqualKey=equal_to<Key>, typename Alloc=alloc>
class hash_multimap {
private:
	typedef hashtable<pair<const Key, Tp>, Key, HashFun, Select1st<pair<const Key, Tp>>, EqualKey, Alloc> Ht;
	Ht m_ht;

public:
	typedef typename Ht::key_type key_type;
	typedef Tp data_type;
	typedef Tp mapped_type;
	typedef typename Ht::value_type value_type;
	typedef typename Ht::hasher hasher;
	typedef typename Ht::key_equal key_equal;

	typedef typename Ht::size_type size_type;
	typedef typename Ht::difference_type difference_type;
	typedef typename Ht::pointer pointer;
	typedef typename Ht::const_pointer const_pointer;
	typedef typename Ht::reference reference;
	typedef typename Ht::const_reference const_reference;
	typedef typename Ht::iterator iterator;
	typedef typename Ht::const_iterator const_iterator;
	typedef typename Ht::allocator_type allocator_type;

	inline hasher hash_func()const { return m_ht.hash_fun(); }
	inline key_equal key_eq()const { return m_ht.key_eq(); }
	inline allocator_type get_allocator()const { return m_ht.get_allocator(); }
public:
	hash_multimap() :m_ht(100, hasher(), key_equal(), allocator_type()) {}
	explicit hash_multimap(size_type n) :m_ht(n, hasher(), key_equal(), allocator_type()) {}
	hash_multimap(size_type n, const hasher& hf) :m_ht(n, hf, key_equal(), allocator_type()) {}
	hash_multimap(size_type n, const hasher& hf, const key_equal& eq) :m_ht(n, hf, eq, allocator_type()) {}
	// other construction
	hash_multimap(size_type* first, size_type* last)
		:m_ht(100, hasher(), key_equal(), allocator_type()) {
		m_ht.insert_equal(first, last);
	}
	hash_multimap(size_type* first, size_type* last, size_type n)
		:m_ht(n, hasher(), key_equal(), allocator_type()) {
		m_ht.insert_equal(first, last);
	}
	hash_multimap(size_type* first, size_type* last, size_type n, hasher& hf)
		:m_ht(n, hf, key_equal(), allocator_type()) {
		m_ht.insert_equal(first, last);
	}
	hash_multimap(size_type* first, size_type* last, size_type n, hasher& hf, key_equal& eq)
		:m_ht(n, hf, eq, allocator_type()) {
		m_ht.insert_equal(first, last);
	}
	// construct with iterator
	hash_multimap(const_iterator first, const_iterator last)
		:m_ht(100, hasher(), key_equal(), allocator_type()) {
		m_ht.insert_equal(first, last);
	}
	hash_multimap(const_iterator first, const_iterator last, size_type n)
		:m_ht(n, hasher(), key_equal(), allocator_type()) {
		m_ht.insert_equal(first, last);
	}
	hash_multimap(const_iterator first, const_iterator last, size_type n, hasher& hf)
		:m_ht(n, hf, key_equal(), allocator_type()) {
		m_ht.insert_equal(first, last);
	}
	hash_multimap(const_iterator first, const_iterator last, size_type n, hasher& hf, key_equal& eq)
		:m_ht(n, hf, eq, allocator_type()) {
		m_ht.insert_equal(first, last);
	}
public:
	inline size_type size()const { return m_ht.size(); }
	inline size_type max_size()const { return m_ht.max_size(); }
	inline bool empty()const { return m_ht.empty(); }
	inline void swap(hash_multimap& hp) { m_ht.swap(hp.m_ht); }
	// iterator
	inline iterator begin() { return m_ht.begin(); }
	inline iterator end() { return m_ht.end(); }
	inline const_iterator begin()const { return m_ht.begin(); }
	inline const_iterator end()const { return m_ht.end(); }
	//
	inline iterator insert(const value_type& obj) {
		return m_ht.insert_equal(obj);
	}
	inline void insert(const value_type* first, const value_type* last) {
		m_ht.insert_equal(first, last);
	}
	inline iterator find(const key_type& key) { return m_ht.find(key); }
	inline const_iterator find(const key_type& key)const { return m_ht.find(key); }
	inline Tp& operator[](const key_type& key) {
		return m_ht.find_or_insert(value_type(key, Tp())).second;
	}
	inline size_type count(const key_type& key)const { return m_ht.count(key); }

	inline pair<iterator, iterator>equal_range(const key_type& key) {
		return m_ht.equal_range(key);
	}
	inline pair<const_iterator, const_iterator>equal_range(const key_type& key) const {
		return m_ht.equal_range(key);
	}

	inline size_type erase(const key_type& key) { return m_ht.erase(key); }
	inline void erase(iterator it) { m_ht.erase(it); }
	inline void erase(iterator first, iterator last) { m_ht.erase(first, last); }

	inline void resize(size_type n) { m_ht.resize(n); }
	inline size_type bucket_count()const { return m_ht.bucket_count(); }
	inline size_type max_bucket_count()const { return m_ht.max_bucket_count(); }
	inline size_type ele_in_bucket(size_type n)const { return m_ht.ele_in_bucket(); }
};
// override operator
template<typename Key, typename Tp, typename HashFun, typename EqualKey, typename Alloc>
inline bool operator==(const hash_multimap<Key, Tp, HashFun, EqualKey, Alloc>& x,
	                   const hash_multimap<Key, Tp, HashFun, EqualKey, Alloc>& y) {
	return x.m_ht == y.m_ht;
}
template<typename Key, typename Tp, typename HashFun, typename EqualKey, typename Alloc>
inline bool operator!=(const hash_multimap<Key, Tp, HashFun, EqualKey, Alloc>& x,
	                   const hash_multimap<Key, Tp, HashFun, EqualKey, Alloc>& y) {
	return !(x.m_ht == y.m_ht);
}
NAMESPACE_END
#endif // H_HASH_MAP_H