#ifndef STL_MAP_H
#define STL_MAP_H

#include "stl_config.h"
#include "stl_alloc.h"
#include "stl_function.h"
#include "stl_tree.h"

NAMESPACE_START
template<typename Key, typename Tp, typename Compare=less<Key>,typename Alloc=alloc>
class map {
public:
	typedef Key key_type;
	typedef Tp data_type;
	typedef Tp mapped_type;
	typedef pair<const Key, Tp> value_type;
	typedef Compare key_compare;
	// value_compare
	class value_compare :public binary_function<value_type, value_type, bool> {
		friend class map<Key, Tp, Compare, Alloc>;
	protected:
		Compare cmp;
		value_compare(Compare c) :cmp(c) {}
	public:
		inline bool operator()(const value_type& x, const value_type& y)const {
			return cmp(x.first, y.first);
		}
	};

private:
	typedef RB_Tree<key_type, value_type, Select1st<value_type>, key_compare, Alloc> RB_type;
	RB_type m_rbt;

public:
	typedef typename RB_type::pointer pointer;
	typedef typename RB_type::const_pointer const_pointer;
	typedef typename RB_type::reference reference;
	typedef typename RB_type::const_reference const_reference;
	typedef typename RB_type::iterator iterator;
	typedef typename RB_type::const_iterator const_iterator;
	typedef typename RB_type::reverse_iterator reverse_iterator;
	typedef typename RB_type::const_reverse_iterator const_reverse_iterator;
	typedef typename RB_type::size_type size_type;
	typedef typename RB_type::difference_type difference_type;
	typedef typename RB_type::allocator_type allocator_type;

	map() :m_rbt(Compare()) {}

	explicit map(const Compare& c) :m_rbt(c) {}

	map(const value_type* first, value_type* last) :m_rbt(Compare()) {
		m_rbt.insert_unique(first, last);
	}

	map(const iterator first, const iterator last) :m_rbt(Compare()) {
		m_rbt.insert_unique(first, last);
	}
	map(const iterator first, const iterator last, Compare& c) :m_rbt(c) {
		m_rbt.insert_equal(first, last);
	}

	map(map& m) :m_rbt(m.m_rbt) {}
	// operator
	friend bool operator==(map&, map&);
	friend bool operator<(map&, map&);

	inline map& operator=(const map& m) {
		if (this != &m)
			m_rbt = m.m_rbt;
		return *this;
	}

	inline key_compare key_cmp()const { return m_rbt.key_cmp(); }
	inline value_compare val_cmp()const { return value_compare(m_rbt.key_cmp()); }
	inline allocator_type get_allocator()const { return m_rbt.get_allocator(); }

	inline iterator begin() { return m_rbt.begin(); }
	inline const_iterator begin()const { return m_rbt.begin(); }
	inline iterator end() { return m_rbt.end(); }
	inline const_iterator end()const { return m_rbt.end(); }
	inline reverse_iterator rbegin() { return m_rbt.rbegin(); }
	inline const_reverse_iterator rbegin()const { return m_rbt.rbegin(); }
	inline reverse_iterator rend() { return m_rbt.rend(); }
	inline const_reverse_iterator rend()const { return m_rbt.rend(); }

	inline bool empty()const { return m_rbt.empty(); }
	inline size_type size()const { return m_rbt.size(); }
	inline size_type max_size()const { return m_rbt.max_size(); }

	inline Tp& operator[](const key_type& k) {
		iterator it = lower_bound(k);
		if (it == end() || key_cmp()(k, (*it).first))
			it = insert(it, value_type(k, Tp()));
		return (*it).second;
	}
	inline void swap(map& x) {
		m_rbt.swap(x.m_rbt);
	}

	// insert and erase
	inline pair<iterator, bool>insert(const value_type& x) {
		return m_rbt.insert_unique(x);}
	inline iterator insert(iterator pos, const value_type& x) {
		return m_rbt.insert_unique(pos, x);}
	inline void insert(const value_type* first, const value_type* last) {
		m_rbt.insert_unique(first, last);}
	inline void insert(const_iterator first, const_iterator last) {
		m_rbt.insert_unique(first, last);}

	inline void erase(iterator pos) { m_rbt.erase(pos); }

	inline size_type erase(const key_type& x) { return m_rbt.erase(x); }

	inline void erase(iterator first, iterator last) {
		m_rbt.erase(first, last);}

	inline void clear() { m_rbt.clear(); }

	inline iterator find(const key_type& x) { return m_rbt.find(x); }
	inline const_iterator find(const key_type& x)const { return m_rbt.find(x); }
	inline size_type count(key_type& x)const { return m_rbt.find(x) == m_rbt.end() ? 0 : 1; }

	inline iterator lower_bound(const key_type& x)  { return m_rbt.lower_bound(x); }
	inline const_iterator lower_bound(const key_type& x) const { return m_rbt.lower_bound(x); }

	inline iterator upper_bound(const key_type& x) { return m_rbt.upper_bound(x); }
	inline const_iterator upper_bound(const key_type& x) const { return m_rbt.upper_bound(x); }

	inline pair<iterator, iterator>equal_range(const key_type& x) { return m_rbt.equal_range(x); }
	inline pair<const_iterator, const_iterator>equal_range(const key_type& x) const{ return m_rbt.equal_range(x); }
};
// operator
template<typename Key, typename Tp, typename Compare, typename Alloc>
inline bool operator==(const map<Key, Tp, Compare, Alloc>& x,
	const map<Key, Tp, Compare, Alloc>& y) {
	return x.m_rbt == y.m_rbt; }

template<typename Key, typename Tp, typename Compare, typename Alloc>
inline bool operator<(const map<Key, Tp, Compare, Alloc>& x,
	const map<Key, Tp, Compare, Alloc>& y) {
	return x.m_rbt < y.m_rbt; }
NAMESPACE_END
#endif // STL_MAP_H