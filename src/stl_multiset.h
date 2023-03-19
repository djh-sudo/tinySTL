#ifndef STL_MULTISET_H
#define STL_MULTISET_H
#include "stl_config.h"
#include "stl_function.h"
#include "stl_tree.h"

NAMESPACE_START
template<typename Key, typename Compare = less<Key>, typename Alloc = alloc>
class multiset {
public:
	typedef Key key_type;
	typedef Key value_type;
	typedef Compare key_compare;
	typedef Compare value_compare;

private:
	typedef RB_Tree<key_type, value_type, Identity<value_type>, key_compare, Alloc> RB_type;
	RB_type m_rbt;
public:
	typedef typename RB_type::const_pointer pointer;
	typedef typename RB_type::const_pointer const_pointer;
	typedef typename RB_type::reference reference;
	typedef typename RB_type::const_reference const_reference;
	typedef typename RB_type::const_iterator iterator;
	typedef typename RB_type::const_iterator const_iterator;
	typedef typename RB_type::const_reverse_iterator reverse_iterator;
	typedef typename RB_type::const_reverse_iterator const_reverse_iterator;
	typedef typename RB_type::size_type size_type;
	typedef typename RB_type::difference_type difference_type;
	typedef typename RB_type::allocator_type allocator_type;

	multiset() :m_rbt(Compare()) {}
	explicit multiset(const Compare& c) :m_rbt(c) {}

	multiset(const value_type* first, const value_type* last) :m_rbt(Compare()) {
		m_rbt.insert_equal(first, last);
	}

	multiset(const iterator first, const iterator last) :m_rbt(Compare()) {
		m_rbt.insert_equal(first, last);
	}

	multiset(const iterator first, const iterator last, Compare& c) :m_rbt(c) {
		m_rbt.insert_equal(first, last);
	}

	multiset(multiset& s) :m_rbt(s.m_rbt) {}

	~multiset() = default;
	// operator
	friend bool operator==(multiset& x, multiset& y);
	friend bool operator<(multiset& x, multiset& y);

	inline multiset& operator=(const multiset& x) {
		m_rbt = x.m_rbt;
		return*this;
	}
	inline key_compare key_cmp()const { return m_rbt.m_key_compare(); }
	inline value_compare val_cmp()const { return m_rbt.m_key_compare(); }
	inline allocator_type get_allocator()const { return m_rbt.get_allocator(); }
	inline iterator begin()const { return m_rbt.begin(); }
	inline iterator end()const { return m_rbt.end(); }
	inline reverse_iterator rbegin()const { return m_rbt.rbegin(); }
	inline reverse_iterator rend()const { return m_rbt.rend(); }
	inline bool empty()const { return m_rbt.empty(); }
	inline size_type size()const { return m_rbt.size(); }
	inline size_type max_size()const { return m_rbt.max_size(); }

	inline void swap(multiset& x) {
		m_rbt.swap(x.m_rbt);
	}
	// insert and erase
	inline iterator insert(const value_type& obj) {
		return m_rbt.insert_equal(obj);
	}
	inline iterator insert(iterator pos, const value_type& obj) {
		typedef typename RB_type::iterator it;
		return m_rbt.insert_equal((it&)pos, obj);
	}
	inline void insert(const_iterator first, const_iterator last) {
		return m_rbt.insert_equal(first, last);
	}
	inline void insert(const value_type* first, const value_type* last) {
		return m_rbt.insert_equal(first, last);
	}
	// erase
	inline void erase(iterator pos) {
		typedef typename RB_type::iterator it;
		m_rbt.erase((it&)pos);
	}
	inline size_type erase(const key_type& x) {
		return m_rbt.erase(x);
	}
	inline void erase(iterator first, iterator last) {
		typedef typename RB_type::iterator it;
		m_rbt.erase((it&)first, (it&)last);
	}
	inline void clear() { return m_rbt.clear(); }
	// multiset option
	inline iterator find(const key_type& key)const { return m_rbt.find(key); }

	inline size_type count(const key_type& x)const {
		return m_rbt.find(x) == m_rbt.end() ? 0 : 1;
	}

	inline iterator lower_bound(const key_type& x)const {
		return m_rbt.lower_bound(x);
	}

	inline iterator upper_bound(const key_type& x)const {
		return m_rbt.upper_bound(x);
	}
	inline pair<iterator, iterator>equal_range(const key_type& x) {
		return m_rbt.equal_range(x);
	}
};

// compare
template<typename Key, typename Compare, typename Alloc>
inline bool operator==(const multiset<Key, Compare, Alloc>& s1,
	const multiset<Key, Compare, Alloc>& s2) {
	return s1.m_rbt == s2.m_rbt;
}

template<typename Key, typename Compare, typename Alloc>
inline bool operator<(const multiset<Key, Compare, Alloc>& s1,
	const multiset<Key, Compare, Alloc>& s2) {
	return s1.m_rbt < s2.m_rbt;
}

NAMESPACE_END
#endif // STL_MULTISET_H