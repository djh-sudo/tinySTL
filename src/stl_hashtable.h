#ifndef STL_HASHTABLE_H
#define STL_HASHTABLE_H

#include <stddef.h>
#include <algorithm>
#include "stl_config.h"
#include "stl_hash_fun.h"
#include "stl_construct.h"
#include "stl_vector.h"
#include "stl_algobase.h"
#include "stl_alloc.h"
#include "stl_pair.h"


NAMESPACE_START
template<typename Val>
struct hashtable_node {
	Val m_val;
	hashtable_node* m_next;
};

template<typename Val, typename Key, typename HashFun, typename ExtractKey, typename EqualKey, typename Alloc = alloc>
class hashtable;

template<typename Val, typename Key, typename HashFun, typename ExtractKey, typename EqualKey, typename Alloc = alloc>
struct hashtable_const_iterator;

template<typename Val, typename Key, typename HashFun, typename ExtractKey, typename EqualKey, typename Alloc = alloc>
struct hashtable_iterator {
	typedef hashtable<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>                Hashtable;
	typedef hashtable_iterator<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>       iterator;
	typedef hashtable_const_iterator<Val, Key, HashFun, ExtractKey, EqualKey, Alloc> const_iterator;
	typedef hashtable_node<Val>  Node;
	typedef forward_iterator_tag iterator_category;
	typedef Val                  value_type;
	typedef ptrdiff_t            difference_type;
	typedef size_t               size_type;
	typedef Val& reference;
	typedef Val* pointer;

	// API
	hashtable_iterator(Node* n, Hashtable* ht) :m_cur(n), m_ht(ht) {}
	hashtable_iterator() = default;
	hashtable_iterator(const hashtable_iterator& x) :m_cur(x.m_cur), m_ht(x.m_ht) {}
	// override operator
	inline reference operator*()const { return m_cur->m_val; }
	inline pointer operator->()const { return &(operator*()); }
	iterator& operator++() {
		const Node* old = m_cur;
		m_cur = m_cur->m_next;
		if (m_cur != nullptr) {
			size_type bucket = m_ht->bucket_num(old->m_val);
			while (!m_cur && ++bucket < m_ht->size()){
				m_cur = m_ht->m_buckets[bucket];
			}
		}
		return*this;
	}
	inline iterator operator++(int) {
		iterator tmp = *this;
		++*this;
		return tmp;
	}
	inline bool operator==(const iterator& it) const { return m_cur == it.m_cur; }
	inline bool operator!=(const iterator& it)const { return m_cur != it.m_cur; }
	// data
	Node* m_cur;
	Hashtable* m_ht;
};

template<typename Val, typename Key, typename HashFun, typename ExtractKey, typename EqualKey, typename Alloc>
struct hashtable_const_iterator {
	typedef hashtable<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>                Hashtable;
	typedef hashtable_iterator<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>       iterator;
	typedef hashtable_const_iterator<Val, Key, HashFun, ExtractKey, EqualKey, Alloc> const_iterator;
	typedef hashtable_node<Val>  Node;
	typedef forward_iterator_tag iterator_category;
	typedef Val                  value_type;
	typedef ptrdiff_t            difference_type;
	typedef size_t               size_type;
	typedef const Val& reference;
	typedef const Val* pointer;

	// API
	hashtable_const_iterator() = default;
	hashtable_const_iterator(const Node* n, Hashtable* ht) :m_cur(n), m_ht(ht) {}
	hashtable_const_iterator(hashtable_const_iterator& x) :m_cur(x.m_cur), m_ht(x.m_ht) {}
	// override operator
	inline reference operator*()const { return m_cur->m_val; }
	inline pointer operator->()const { return &(operator*()); }
	const_iterator& operator++() {
		const Node* old = m_cur;
		m_cur = m_cur->m_next;
		if (m_cur != nullptr) {
			size_type bucket = m_ht->bucket_num(old->m_val);
			while (!m_cur && ++bucket < m_ht->size()) {
				m_cur = m_ht->m_buckets[bucket];
			}
		}
		return*this;
	}
	inline const_iterator operator++(int) {
		const_iterator tmp = *this;
		++* this;
		return tmp;
	}
	inline bool operator==(const_iterator& it) const { return m_cur == it.m_cur; }
	inline bool operator!=(const_iterator& it)const { return m_cur != it.m_cur; }
	// data
	Node* m_cur;
	Hashtable* m_ht;
};

template<typename Val, typename Key, typename HashFun, typename ExtractKey, typename EqualKey, typename Alloc>
inline forward_iterator_tag
iterator_category(const hashtable_iterator<Val, Key, HashFun,ExtractKey, EqualKey,Alloc>&) {
	return forward_iterator_tag();
}

template<typename Val, typename Key, typename HashFun, typename ExtractKey, typename EqualKey, typename Alloc>
inline Val*
value_type(const hashtable_iterator<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>&) {
	return (Val*)nullptr; }

template<typename Val, typename Key, typename HashFun, typename ExtractKey, typename EqualKey, typename Alloc>
inline typename hashtable<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>::difference_type*
distance_type(const hashtable_iterator<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>&) {
	return (typename hashtable<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>::difference_type*) nullptr;
}

// hashtable
enum { PRIMES_NUMBER = 28 };
static const unsigned long prime_list[PRIMES_NUMBER] = {
 53ul,         97ul,         193ul,       389ul,       769ul,
 1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
 49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
 1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
 50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
 1610612741ul, 3221225473ul, 4294967291ul };

inline unsigned long next_prime(unsigned long n) {
	const unsigned long* first = prime_list;
	const unsigned long* last = prime_list + (unsigned long)PRIMES_NUMBER;
	// ...
	const unsigned long* pos = std::lower_bound(first, last, n);
	return pos == last ? *(last - 1) : *pos;
}
template<typename Val, typename Key, typename HashFun,
	     typename ExtractKey, typename EqualKey, typename Alloc>
class hashtable {
public:
	typedef Key      key_type;
	typedef Val      value_type;
	typedef HashFun  hasher;
	typedef EqualKey key_equal;

	typedef size_t            size_type;
	typedef ptrdiff_t         difference_type;
	typedef value_type*       pointer;
	typedef const value_type* const_pointer;
	typedef value_type&       reference;
	typedef const value_type& const_reference;

public:
	inline hasher hash_fun()const { return m_hash; }
	inline key_equal key_eq()const { return m_equal; }

public:
	typedef Alloc allocator_type;
	inline allocator_type get_allocator()const { return allocator_type(); }

private:
	typedef hashtable_node<Val> Node;
	typedef SimpleAlloc<Node, Alloc> node_allocator_type;

	inline Node* get_node() { return node_allocator_type::allocate(1); }
	inline void put_node(Node* p) { return node_allocator_type::deallocate(p); }

public:
	typedef hashtable_iterator<Val, Key, HashFun, ExtractKey, EqualKey, Alloc> iterator;
	typedef hashtable_const_iterator<Val, Key, HashFun, ExtractKey, EqualKey, Alloc> const_iterator;
	friend struct hashtable_iterator<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>;
	friend struct hashtable_const_iterator<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>;

public:
	hashtable(size_type n, 
		      const HashFun&hf,
		      const EqualKey& eql,
		      const ExtractKey & ext,
		      const allocator_type& a=allocator_type())
		:m_hash(hf),m_equal(eql),m_get_key(ext),m_buckets(a),m_num_ele(0) {
		initialize_buckets(n);
	}
	hashtable(size_type n,
		const HashFun& hf,
		const EqualKey& eql,
		const allocator_type& a = allocator_type())
		:m_hash(hf), m_equal(eql), m_get_key(ExtractKey()), m_buckets(a), m_num_ele(0) {
		initialize_buckets(n);
	}
	hashtable(const hashtable& x)
		:m_hash(x.m_hash),m_equal(x.m_equal),
		m_get_key(x.m_get_key),m_num_ele(x.m_num_ele),
		m_buckets(x.get_allocator()),m_num_ele(0) {
		copy_from(x);
	}
	inline hashtable& operator=(const hashtable& ht) {
		if (ht != *this) {
			clear();
			m_hash = ht.m_hash;
			m_equal = ht.m_equal;
			m_get_key = ht.m_get_key;
			copy_from(ht);
		}
		return*this;
	}

	~hashtable() { clear(); }
	inline size_type size()const { return m_num_ele; }
	inline size_type max_size()const { return size_type(-1); }
	inline bool empty()const { return m_num_ele == 0; }
	inline void swap(const hashtable& ht) {
		std::swap(m_hash, ht.m_hash);
		std::swap(m_equal, ht.m_equal);
		std::swap(m_get_key, ht.m_get_key);
		m_buckets.swap(ht.m_buckets);
		std::swap(m_num_ele, ht.m_num_ele);
	}
	
	iterator begin() {
		size_type length = m_buckets.size();
		for (size_type n = 0; n < length; ++n) {
			if (m_buckets[n] != nullptr) return iterator(m_buckets[n], this);
		}
		return end();
	}

	inline iterator end() { return iterator(nullptr, this); }
	
	const_iterator begin()const {
		for (size_type n = 0; n < m_buckets; ++n) {
			if (m_buckets[n]) return const_iterator(m_buckets[n], this);
		}
		return end();
	}

	inline const_iterator end()const { return const_iterator(nullptr, this); }
	// delete
	iterator rbgein() = delete;
	iterator rend() = delete;
	const_iterator rbegin()const = delete;
	const_iterator rend()const = delete;

	inline size_type bucket_count()const { return m_buckets.size(); }
	inline size_type max_bucket_count()const { return prime_list[(size_type)PRIMES_NUMBER - 1]; }

	size_type ele_in_bucket(size_type bucket) const {
		size_type res = 0;
		for (Node* cur = m_buckets[bucket]; cur != nullptr; cur = cur->m_next) {
			++res;
		}
		return res;
	}
	
	reference find_or_insert(const value_type& obj) {
		resize(m_num_ele + 1);
		size_type n = bucket_num(obj);
		Node* first = m_buckets[n];
		for (Node* cur = first; cur != nullptr; cur = cur->m_next) {
			if (m_equal(m_get_key(cur->m_val), m_get_key(obj))) return cur->m_val;
		}
		Node* tmp = new_node(obj);
		tmp->m_next = first;
		m_buckets[n] = tmp;
		++m_num_ele;
		return tmp->m_val;
	}

	iterator find(const key_type& key) {
		size_type n = bucket_num_key(key);
		Node* first = nullptr;
		for (first = m_buckets[n]; 
			 first && m_equal(m_get_key(first->m_val), key);
			 first = first->m_next);
		return iterator(first, this);
	}
	const_iterator find(const key_type& key) const{
		size_type n = bucket_num_key(key);
		Node* first = nullptr;
		for (first = m_buckets[n];
			 first && m_equal(m_get_key(first->m_val), key);
			 first = first->m_next);
		return const_iterator(first, this);
	}

	pair<iterator, iterator>equal_range(const key_type& key) {
		typedef pair<iterator, iterator>pii;
		const size_type n = bucket_num_key(key);
		for (Node* first = m_buckets[n]; first != nullptr; first = first->m_next) {
			if (m_equal(m_get_key(first->m_val), key)) {
				for (Node* cur = first; cur != nullptr; cur = cur->m_next) {
					if (!m_equal(m_get_key(first->m_val), key))
						return pii(iterator(first, this), iterator(cur, this));
				}
				size_type length = m_buckets.size();
				for (size_type m = n + 1; m < length; ++m) {
					if (m_buckets[m] != nullptr)
						return pii(iterator(first, this), iterator(m_buckets[m], this));
				}
				return pii(iterator(first, this), iterator(end(), this));
			}
		}
		return pii(end(), end());
	}

	pair<const_iterator, const_iterator>equal_range(const key_type& key) const{
		typedef pair<const_iterator, const_iterator>pii;
		const size_type n = bucket_num_key(key);
		for (Node* first = m_buckets[n]; first != nullptr; first = first->m_next) {
			if (m_equal(m_get_key(first->m_val), key)) {
				for (Node* cur = first; cur != nullptr; cur = cur->m_next) {
					if (!m_equal(m_get_key(first->m_val), key))
						return pii(const_iterator(first, this), const_iterator(cur, this));
				}
				size_type length = m_buckets.size();
				for (size_type m = n + 1; m < length; ++m) {
					if (m_buckets[m] != nullptr)
						return pii(const_iterator(first, this), const_iterator(m_buckets[m], this));
				}
				return pii(const_iterator(first, this), const_iterator(end(), this));
			}
		}
		return pii(end(), end());
	}

	size_type count(const key_type& key)const {
		const size_type n = bucket_num_key(key);
		size_type res = 0;
		for (const Node* cur = m_buckets[n]; cur != nullptr; cur = cur->m_next) {
			if (m_equal(m_get_key(cur->m_val), key)) ++res;
		}
		return res;
	}
	
	void resize(size_type hint) {
		const size_type old_n = m_buckets.size();
		if (hint > old_n) {
			const size_type n = next_size(hint);
			if (n > old_n) {
				vector<Node*, Alloc> b(n);
				for (size_type bucket = 0; bucket < old_n; ++bucket) {
					Node* first = m_buckets[bucket];
					while (first != nullptr) {
						size_type new_b = bucket_num(first->m_val, n);
						m_buckets[bucket] = first->m_next;
						first->m_next = b[new_b];
						b[new_b] = first;
						first = m_buckets[bucket];
					}
				}
				m_buckets.swap(b);
			}
		}
	}
	
	inline pair<iterator, bool> insert_unique(const value_type& obj) {
		resize(m_num_ele + 1);
		return insert_unique_noresize(obj);
	}
	// insert
	inline iterator insert_equal(const value_type& obj) {
		resize(m_num_ele + 1);
		return insert_equal_norsize(obj);
	}
	void insert_unique(const value_type* first, const value_type* last) {
		size_type n = last - first;
		resize(m_num_ele + n);
		for (; n > 0; --n, ++first) {
			insert_unique_noresize(*first);
		}
	}

	void insert_unique(const iterator first, const iterator last) {
		size_type n = 0;
		distance(first, last, n);
		resize(m_num_ele + n);
		for (; n > 0; --n, ++first) {
			insert_unique_noresize(*first);
		}
	}

	void insert_equal(const value_type* first, const value_type* last) {
		size_type n = last - first;
		resize(m_num_ele + n);
		for (; n > 0; --n, ++first) {
			insert_equal_norsize(*first);
		}
	}
	
	void insert_equal(const iterator first, const iterator last) {
		size_type n = 0;
		distance(first, last, 0);
		resize(m_num_ele + n);
		for (; n > 0; --n, ++first) {
			insert_equal_norsize(*first);
		}
	}

	size_type erase(const key_type& key) {
		const size_type n = bucket_num_key(key);
		Node* first = bucket_num_key(key);
		size_type erased = 0;
		if (first) {
			Node* cur = first;
			Node* next = cur->m_next;
			while (next) {
				if (m_equal(m_get_key(next->m_val), key)) {
					cur->m_next = next->m_next;
					delete_node(next);
					next = cur;
					++erased;
					--m_num_ele;
				}
				else {
					cur = next;
					next = cur->m_next;
				}
			}
			if (m_equal(m_get_key(first->m_val), key)) {
				m_buckets[n] = first->m_next;
				delete_node(first);
				++erased;
				--m_num_ele;
			}
		}
		return erased;
	}

	void erase(const iterator& it) {
		Node* p = it.m_cur;
		if (p != nullptr) {
			const size_type n = bucket_num(p->m_val);
			Node* cur = m_buckets[n];
			if (cur == p) {
				m_buckets[n] = cur->m_next;
				delete_node(cur);
				--m_num_ele;
			}
			else {
				Node* next = cur->m_next;
				while (next != nullptr) {
					if (next == p) {
						cur->m_next = next->m_next;
						delete_node(next);
						--m_num_ele;
						break;
					}
					else {
						cur = next;
						next = cur->m_next;
					}
				}
			}
		}
	}

	void erase(const iterator first, const iterator last) {
		if (first.m_cur == last.m_cur) return;
		size_type fb = first.m_cur ? bucket_num(first.m_cur->m_val) : m_buckets.size();
		size_type lb = last.m_cur ? bucket_num(last.m_cur->m_val) : m_buckets.size();
		if (fb == lb) { erase_bucket(fb, first.m_cur); }
		else {
			erase_bucket(fb, first.m_cur, nullptr);
			for (size_type n = fb + 1; n < lb; ++n) {
				erase_bucket(n, nullptr);
			}
			if (lb != m_buckets.size()) {
				erase_bucket(lb, last.m_cur);
			}
		}
	}

	void clear(){
		size_type length = m_buckets.size();
		for (size_type i = 0; i < length; ++i) {
			Node* cur = m_buckets[i];
			while (cur != nullptr) {
				Node* next = cur->m_next;
				delete_node(cur);
				cur = next;
			}
			m_buckets[i] = nullptr;
		}
		m_num_ele = 0;
	}
// auxiliary function
private:
	inline size_type next_size(size_type n)const { return next_prime(n); }
	inline void initialize_buckets(size_type n) {
		const size_type n_bucket = next_prime(n);
		m_buckets.reserve(n_bucket);
		m_buckets.reserve(n_bucket);
		m_buckets.insert(m_buckets.end(), n_bucket, (Node*)nullptr);
		m_num_ele = 0;
	}
	inline size_type bucket_num_key(const key_type& key, size_t n)const {
		return m_hash(key) % n;}
	inline size_type bucket_num_key(const key_type& key)const { 
		return bucket_num_key(key, m_buckets.size()); }
	inline size_type bucket_num(const value_type& obj)const { 
		return bucket_num_key(m_get_key(obj)); }
	inline size_type bucket_num(const value_type&obj, size_t n)const {
		return bucket_num_key(m_get_key(obj), n);}

	inline Node* new_node(const value_type& obj) {
		Node* n = get_node();
		n->m_next = nullptr;
		construct(&n->m_val, obj);
		return n;
	}
	inline void delete_node(Node* n) {
		destroy(&n->m_val);
		put_node(n);
	}
	void erase_bucket(const size_type n, Node* first, Node* last) {
		Node* cur = m_buckets[n];
		if (cur == first) erase_bucket(n, last);
		else {
			Node* next = nullptr;
			for (next = cur->m_next; next != first; cur = next, next = cur->m_next);
			while (next != last) {
				cur->m_next = next->m_next;
				delete_node(next);
				next = cur->m_next;
				--m_num_ele;
			}
		}
	}
	void erase_bucket(const size_type n, Node* last) {
		Node* cur = m_buckets[n];
		while (cur != last) {
			Node* next = cur->m_next;
			delete_node(cur);
			cur = next;
			m_buckets[n] = cur;
			--m_num_ele;
		}
	}
	void copy_from(const hashtable& ht) {
		m_buckets.clear();
		m_buckets.reserve(ht.m_buckets.size());
		m_buckets.insert(m_buckets.end(), ht.m_buckets.size(), (Node*)nullptr);
		size_type length = ht.m_buckets.size();
		for (size_type i = 0; i < length; ++i) {
			const Node* cur = ht.m_buckets[i];
			if (cur != nullptr) {
				Node* copy = new_node(cur->m_val);
				m_buckets[i] = copy;
				// slow path
				for (Node* next = cur->m_next; next != nullptr; cur = next, next = cur->m_next) {
					copy->m_next = new_node(next->val);
					copy = copy->m_next;
				}
				copy->m_next = nullptr;
			}
		}
		m_num_ele = ht.m_num_ele;
	} 
	// insert auxiliary
	pair<iterator, bool> insert_unique_noresize(const value_type& obj) {
		const size_type n = bucket_num(obj);
		Node* first = m_buckets[n];
		for (Node* cur = first; cur; cur = cur->m_next) {
			if (m_equal(m_get_key(cur->m_val), m_get_key(obj))) {
				return pair<iterator, bool>(iterator(cur, this), false);}
		}
		Node* tmp = new_node(obj);
		tmp->m_next = first; 
		m_buckets[n] = tmp;
		++m_num_ele;
		return pair<iterator, bool>(iterator(tmp, this), true);
	}
	
	iterator insert_equal_norsize(const value_type& obj) {
		const size_type n = bucket_num(obj);
		Node* first = m_buckets[n];
		for (Node* cur = first; cur; cur = cur->m_next) {
			if (m_equal(m_get_key(cur->m_val), m_get_key(obj))) {
				Node* tmp = new_node(obj);
				tmp->m_next = cur->m_next;
				cur->m_next = tmp;
				++m_num_ele;
				return iterator(tmp, this);
			}
		}
		Node* tmp = new_node(obj);
		tmp->m_next = first;
		m_buckets[n] = tmp;
		++m_num_ele;
		return iterator(tmp, this);
	}


private:
	hasher m_hash;
	key_equal m_equal;
	ExtractKey m_get_key;
	vector<Node*, Alloc> m_buckets;
	size_type m_num_ele;
};
// override operator
template<typename Val, typename Key, typename HashFun,
	     typename ExtractKey, typename EqualKey, typename Alloc>
bool operator==(const hashtable<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>&h1,
	            const hashtable<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>&h2){
	typedef typename hashtable<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>::Node Node;
	if (h1.m_buckets.size() != h2.m_buckets.size()) return false;
	size_t length = h1.m_buckets.size();
	for (size_t n = 0; n < length; ++n) {
		Node* cur1 = h1.m_buckets[n];
		Node* cur2 = h2.m_buckets[n];
		for (; cur1 && cur2 && cur1->m_val == cur2->m_val; 
			   cur1 = cur1->m_next, cur2 = cur2->m_next) {}
		if (cur1 || cur2) return false;
	}
	return true;
}

template<typename Val, typename Key, typename HashFun,
	typename ExtractKey, typename EqualKey, typename Alloc>
inline bool operator!=(const hashtable<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>& h1,
	            const hashtable<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>& h2) {
	return !(h1 == h2);
}

template<typename Val, typename Key, typename HashFun,
	typename ExtractKey, typename EqualKey, typename Alloc>
inline void swap(const hashtable<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>& h1,
	             const hashtable<Val, Key, HashFun, ExtractKey, EqualKey, Alloc>& h2) {
	return h1.swap(h2);
}


NAMESPACE_END
#endif