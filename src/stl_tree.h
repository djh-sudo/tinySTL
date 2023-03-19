#ifndef STL_TREE_H
#define STL_TREE_H

#include <stddef.h>
#include "stl_config.h"
#include "stl_algobase.h"
#include "stl_alloc.h"
#include "stl_construct.h"
#include "stl_iterator_base.h"
#include "stl_function.h"

NAMESPACE_START

typedef bool RB_TREE_COLOR_TYPE;
const RB_TREE_COLOR_TYPE _RB_RED = false;
const RB_TREE_COLOR_TYPE _RB_BLACK = true;

struct RB_Tree_Node_Base {
	typedef RB_TREE_COLOR_TYPE Color;
	typedef RB_Tree_Node_Base* Base_ptr;

	Color m_color;
	Base_ptr m_parent;
	Base_ptr m_left;
	Base_ptr m_right;

	static Base_ptr minimum(Base_ptr x) {
		while (x->m_left != nullptr)x = x->m_left;
		return x;
	}

	static Base_ptr maxumum(Base_ptr x) {
		while (x->m_right != nullptr) x = x->m_right;
		return x;
	}
};

// RB Tree Node
template<typename Val>
struct RB_Tree_Node :public RB_Tree_Node_Base{
	typedef RB_Tree_Node<Val>* link_type;
	Val m_val;
};

// base iterator
struct RB_Tree_Base_Iterator {
	typedef RB_Tree_Node_Base::Base_ptr Base_ptr;
	typedef bidirectional_iterator_tag iterator_category;
	typedef ptrdiff_t difference_type;
	Base_ptr m_node;

	void increment() {
		if (m_node->m_right != nullptr) {
			m_node = m_node->m_right;
			while (m_node->m_left != nullptr) { m_node = m_node->m_left; }
		}
		else {
			Base_ptr p = m_node->m_parent;
			while (m_node == p->m_right) {
				m_node = p;
				p = p->m_parent;
			}
			if (m_node->m_right != p)
				m_node = p;
		}
	}
	void decrement() {
		if (m_node->m_color == _RB_RED &&
			m_node->m_parent->m_parent == m_node) {
			m_node = m_node->m_right;
		}
		else if (m_node->m_left != nullptr) {
			Base_ptr p = m_node->m_left;
			while (p->m_right != nullptr) {
				p = p->m_right;
			}
			m_node = p;
		}
		else {
			Base_ptr p = m_node->m_parent;
			while (p->m_left == m_node) {
				m_node = p;
				p = p->m_parent;
			}
			m_node = p;
		}
	}
};
// RB Tree iterator
template<typename Val, typename Ref, typename Ptr>
struct RB_Tree_Iterator :public RB_Tree_Base_Iterator {
	typedef Val value_type;
	typedef Ref reference;
	typedef Ptr pointer;
	typedef RB_Tree_Iterator<Val, Val&, Val*> iterator;
	typedef RB_Tree_Iterator<Val, const Val&, const Val*> const_iterator;
	typedef RB_Tree_Iterator<Val, Ref, Ptr>Self;
	typedef RB_Tree_Node<Val>* link_type;

	RB_Tree_Iterator() = default;
	RB_Tree_Iterator(link_type x) { m_node = x; }
	RB_Tree_Iterator(const iterator& it) { m_node = it.m_node; }

	inline reference operator*() const { return (link_type(m_node))->m_val; }
	inline Self& operator++() { increment(); return *this; }
	inline Self operator++(int) {
		Self tmp = *this;
		increment();
		return tmp;
	}
	inline Self& operator--() { decrement(); return *this; }
	inline Self operator--(int) {
		Self tmp = *this;
		decrement();
		return tmp;
	}
};
// overload operator
inline bool operator==(const RB_Tree_Base_Iterator& x,
	                   const RB_Tree_Base_Iterator& y) {
	return x.m_node == y.m_node;
}
inline bool operator!=(const RB_Tree_Base_Iterator& x,
	                   const RB_Tree_Base_Iterator& y) {
	return x.m_node != y.m_node;
}
inline bidirectional_iterator_tag
iterator_category(const RB_Tree_Base_Iterator&) {
	return bidirectional_iterator_tag();
}

inline RB_Tree_Base_Iterator::difference_type*
distance_type(RB_Tree_Base_Iterator&) {
	return (RB_Tree_Base_Iterator::difference_type*)nullptr;
}

template<typename Val, typename Ref, typename Ptr>
inline Val* value_type(RB_Tree_Iterator<Val, Ref, Ptr>&) {
	return (Val*)nullptr;
}
// auxiliary function
inline void rotate_left(RB_Tree_Node_Base* x, RB_Tree_Node_Base*& root) {
	RB_Tree_Node_Base* y = x->m_right;
	x->m_right = y->m_left;
	if (y->m_left != nullptr) y->m_left->m_parent = x;
	y->m_parent = x->m_parent;
	if (x == root)
		root = y;
	else if (x == x->m_parent->m_left)
		x->m_parent->m_left = y;
	else
		x->m_parent->m_right = y;
	y->m_left = x;
	x->m_parent = y;
}

inline void rotate_right(RB_Tree_Node_Base* x, RB_Tree_Node_Base*& root) {
	RB_Tree_Node_Base* y = x->m_left;
	x->m_left = y->m_right;
	if (y->m_right != nullptr) y->m_right->m_parent = x;
	y->m_parent = x->m_parent;

	if (x == root)
		root = y;
	else if (x == x->m_parent->m_right)
		x->m_parent->m_right = y;
	else
		x->m_parent->m_left = y;
	y->m_right = x;
	x->m_parent = y;
}

void rebalence(RB_Tree_Node_Base* x, RB_Tree_Node_Base*& root) {
	x->m_color = _RB_RED;
	while (x != root && x->m_parent->m_color == _RB_RED) {
		if (x->m_parent == x->m_parent->m_parent->m_left) {
			RB_Tree_Node_Base* y = x->m_parent->m_parent->m_right;
			if (y != nullptr && y->m_color == _RB_RED) {
				x->m_parent->m_color = _RB_BLACK;
				y->m_color = _RB_BLACK;
				x->m_parent->m_parent->m_color = _RB_RED;
				// iteration
				x = x->m_parent->m_parent;
			}
			else {
				if (x == x->m_parent->m_right) {
					x = x->m_parent;
					rotate_left(x, root);
				}
				x->m_parent->m_color = _RB_BLACK;
				x->m_parent->m_parent->m_color = _RB_RED;
				rotate_right(x->m_parent->m_parent, root);
				break;
			}
		}
		else {
			RB_Tree_Node_Base* y = x->m_parent->m_parent->m_left;
			if (y != nullptr && y->m_color == _RB_RED) {
				x->m_parent->m_color = _RB_BLACK;
				y->m_color = _RB_BLACK;
				x->m_parent->m_parent->m_color = _RB_RED;
				x = x->m_parent->m_parent;
			}
			else {
				if (x == x->m_parent->m_left) {
					x = x->m_parent;
					rotate_right(x, root);
				}
				x->m_parent->m_color = _RB_BLACK;
				x->m_parent->m_parent->m_color = _RB_RED;
				rotate_left(x->m_parent->m_parent, root);
				break;
			}
		}
	}
	root->m_color = _RB_BLACK;
}

RB_Tree_Node_Base*
rebalence_for_erase(RB_Tree_Node_Base* z,
	                      RB_Tree_Node_Base*& root,
	                      RB_Tree_Node_Base*& left_most,
	                      RB_Tree_Node_Base*& right_most) {
	// z: node to be deleted
	RB_Tree_Node_Base* y = z;
	RB_Tree_Node_Base* x = nullptr;
	RB_Tree_Node_Base* x_parent = nullptr;
	if (y->m_left == nullptr)
		x = y->m_right;
	else {
		if (y->m_right == nullptr) {
			x = y->m_left;
		}
		else {
			// z has two not null children
			y = y->m_right;
			while (y->m_right != nullptr)
				y = y->m_left;
			x = y->m_right;
		}
	}
	// y is successor of z
	// x is right-tree of y (x might null) 
	if (y != z) {
		z->m_left->m_parent = y;
		// y can not own left-tree
		y->m_left = z->m_left;

		if (y != z->m_right) {
			x_parent = y->m_parent;
			if (x != nullptr)
				x->m_parent = y->m_parent;
			y->m_parent->m_left = x;
			y->m_right = z->m_right;
			z->m_right->m_parent = y;
		}
		else {
			// y is right-tree of z
			x_parent = y;
			if (root == z)
				root = y;
			else if (z->m_parent->m_left == z)
				z->m_parent->m_left = y;
			else
				z->m_parent->m_right = y;
			y->m_parent = z->m_parent;
			std::swap(y->m_color, z->m_color);
			y = z;
		}
	}
	else {
		// y equal to z
		// y has at least one sub-tree
		x_parent = y->m_parent;
		if (x != nullptr) x->m_parent = y->m_parent;
		if (root == z)
			root = x;
		else {
			if (z->m_parent->m_left == z)
				z->m_parent->m_left = x;
			else
				z->m_parent->m_right = x;
		}
		if (left_most == z) {
			if (z->m_right == nullptr)
				left_most = z->m_parent;
			else
				left_most = RB_Tree_Node_Base::minimum(x);
		}
		if (right_most == z) {
			if (z->m_left == nullptr)
				right_most = z->m_parent;
			else
				right_most = RB_Tree_Node_Base::maxumum(x);
		}
	}
	// modify color
	if (y->m_color != _RB_RED) {
		// z(y) color is BLACK
		// x_parent is not null
		while (x != root && (x == nullptr || x->m_color == _RB_BLACK)) {
			if (x == x_parent->m_left) {
				RB_Tree_Node_Base* w = x_parent->m_right;
				// w is not null !
				if (w->m_color == _RB_RED) {
					w->m_color = _RB_BLACK;
					x_parent->m_color = _RB_RED;
					rotate_left(x_parent, root);
					w = x_parent->m_right;
				}
				if ((w->m_left == nullptr || w->m_left->m_color == _RB_BLACK) &&
					(w->m_right == nullptr || w->m_right->m_color == _RB_BLACK)) {
					w->m_color = _RB_RED;
					x = x_parent;
					x_parent = x_parent->m_parent;
				}
				else {
					if (w->m_right == nullptr || w->m_right->m_color == _RB_BLACK) {
						if (w->m_left != nullptr) w->m_left->m_color = _RB_BLACK;
						w->m_color = _RB_RED;
						rotate_right(w, root);
						w = x_parent->m_right;
					}
					w->m_color = x_parent->m_color;
					x_parent->m_color = _RB_BLACK;
					if (w->m_right != nullptr) w->m_right->m_color = _RB_BLACK;
					rotate_left(x_parent, root);
					break;
				}
			}
			else {
				RB_Tree_Node_Base* w = x_parent->m_left;
				if (w->m_color == _RB_RED) {
					w->m_color = _RB_BLACK;
					x_parent->m_color = _RB_RED;
					rotate_right(x_parent, root);
					w = x_parent->m_left;
				}
				if ((w->m_right == nullptr || w->m_right->m_color == _RB_BLACK) &&
					(w->m_left == nullptr || w->m_left->m_color == _RB_BLACK)) {
					w->m_color = _RB_RED;
					x = x_parent;
					x_parent = x_parent->m_parent;
				}
				else {
					if (w->m_left == nullptr || w->m_left->m_color == _RB_BLACK) {
						if (w->m_right != nullptr) w->m_right->m_color = _RB_BLACK;
						rotate_left(w, root);
						w = x_parent->m_left;
					}
					w->m_color = x_parent->m_color;
					x_parent->m_color = _RB_BLACK;
					if (w->m_left != nullptr) w->m_left->m_color = _RB_BLACK;
					rotate_right(x_parent, root);
					break;
				}
			}
		}
		if (x != nullptr) x->m_color = _RB_BLACK;
	}
	return y;
}

// RB Tree Base
template<typename Tp, typename Alloc=alloc>
struct RB_Tree_Base {
public:
	typedef Alloc allocator_type;
	inline allocator_type get_allocator()const { return allocator_type(); }
	RB_Tree_Base():m_header(nullptr){
		m_header = get_node();
	}
	~RB_Tree_Base() {
		put_node(m_header);
	}

protected:
	RB_Tree_Node<Tp>* m_header;
	typedef SimpleAlloc<RB_Tree_Node<Tp>, Alloc> alloc_type;

	inline RB_Tree_Node<Tp>* get_node() { return alloc_type::allocate(1); }
	inline void put_node(RB_Tree_Node<Tp>* p) { alloc_type::deallocate(p, 1); }
};

// RB Tree
template<typename Key, typename Val, typename KeyOfValue, typename Compare, typename Alloc=alloc>
class RB_Tree :protected RB_Tree_Base<Val, Alloc> {
private:
	typedef RB_Tree_Base<Val, Alloc> Base;
protected:
	typedef RB_Tree_Node_Base* Base_Ptr;
	typedef RB_Tree_Node<Val> Node;
	typedef RB_TREE_COLOR_TYPE Color_Type;
public:
	typedef Key key_value;
	typedef Val value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef Node* link_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef typename Base::allocator_type allocator_type;
	inline allocator_type get_allocator()const { return Base::get_allocator(); }

protected:
	using Base::get_node;
	using Base::put_node;
	using Base::m_header;

protected:
	inline link_type create_node(const value_type& obj) {
		link_type tmp = get_node();
		construct(&tmp->m_val, obj);
		return tmp;
	}
	inline link_type clone_node(link_type x) {
		link_type tmp = create_node(x->m_val);
		tmp->m_color = x->m_color;
		tmp->m_left = nullptr;
		tmp->m_right = nullptr;
		return tmp;
	}

	inline void destroy_node(link_type p) {
		destroy(&p->m_val);
		put_node(p);
	}
	inline link_type& m_root()const { return (link_type&)(m_header->m_parent); }
	inline link_type& m_left_most()const { return (link_type&)(m_header->m_left); }
	inline link_type& m_right_most()const { return (link_type&)(m_header->m_right); }

	static inline link_type& m_left(link_type x) { return (link_type&)(x->m_left); }
	static inline link_type& m_right(link_type x) { return (link_type&)(x->m_right); }
	static inline link_type& m_parent(link_type x) { return (link_type&)(x->m_parent); }
	static inline reference m_value(link_type x) { return x->m_val; }
	static inline const key_value& m_key(link_type x) { return KeyOfValue()(m_value(x)); }
	static Color_Type& m_color(link_type x) { return (Color_Type&)(x->m_color); }
	// overload function
	static inline link_type& m_left(Base_Ptr x) { return (link_type&)(x->m_left); }
	static inline link_type& m_right(Base_Ptr x) { return (link_type&)(x->m_right); }
	static inline link_type& m_parent(Base_Ptr x) { return (link_type&)(x->m_parent); }
	static inline reference m_value(Base_Ptr x) { return ((link_type)x)->m_val; }
	static inline const key_value& m_key(Base_Ptr x) { return KeyOfValue()(m_value(x)); }
	static inline Color_Type& m_color(Base_Ptr x) { return (Color_Type&)(x->m_color); }

	static inline link_type minimun(link_type x) { return (link_type)RB_Tree_Node_Base::minimum(x); }
	static inline link_type maximun(link_type x) { return (link_type)RB_Tree_Node_Base::maxumum(x); }

protected:
	size_type m_node_count{ 0 };
	Compare m_key_compare;

public:
	typedef RB_Tree_Iterator<value_type, reference, pointer> iterator;
	typedef RB_Tree_Iterator<value_type, const_reference, const_pointer> const_iterator;
	typedef ReverseBidirectionalIter<iterator, value_type, reference, difference_type> reverse_iterator;
	typedef ReverseBidirectionalIter<const_iterator, value_type, const_reference, difference_type> const_reverse_iterator;

public:
	RB_Tree() :Base(allocator_type()), m_node_count(0), m_key_compare() {
		empty_initialize();
	}
	RB_Tree(const Compare& cmp) :Base(), m_node_count(0), m_key_compare(cmp) {
		empty_initialize();
	}
	RB_Tree(const RB_Tree& tree) 
		:Base(tree.get_allocator()), m_node_count(0), m_key_compare(tree.m_key_compare) {
		if (tree.m_root() == nullptr) {
			empty_initialize();
		}
		else {
			m_color(m_header) = _RB_RED;
			m_root() = copy(tree.m_root(), m_header);
			m_left_most() = minimun(m_root());
			m_right_most() = maximun(m_root());
		}
		m_node_count = tree.m_node_count;
	}
	
	~RB_Tree() { clear(); }
	RB_Tree& operator==(RB_Tree& x) {
		if (&x != this) {
			clear();
			m_node_count = 0;
			m_key_compare = x.m_key_compare;
			if (x.m_root() == nullptr) {
				m_root() = nullptr;
				m_left_most() = m_header;
				m_right_most() = m_header;
			}
			else {
				m_root() = copy(x.m_root(), m_header);
				m_left_most() = minimun(m_root());
				m_right_most() = maximun(m_root());
				m_node_count = x.m_node_count;
			}
		}
		return *this;
	}

	inline Compare key_cmp()const { return m_key_compare; }
	inline iterator begin() { return m_left_most(); }
	inline const_iterator begin()const { return m_left_most(); }
	inline iterator end() { return m_header(); }
	inline const_iterator end()const { return m_header; }
	inline reverse_iterator rbegin() { return reverse_iterator(end()); }
	inline const_reverse_iterator rbegin()const { return const_reverse_iterator(end()); }
	inline reverse_iterator rend() { return reverse_iterator(begin()); }
	inline const_reverse_iterator rend()const { return const_reverse_iterator(begin()); }
	
	inline  bool empty()const { return m_node_count == 0; }
	inline size_type size()const { return m_node_count; }
	inline size_type max_size()const { return size_type(-1); }
	inline void swap(RB_Tree& x) {
		std::swap(m_header, x.m_header);
		std::swap(m_node_count, x.m_node_count);
		std::swap(m_key_compare, x.m_key_compare);
	}

public:
	iterator insert_equal(const value_type& v) {
		link_type y = m_header;
		link_type x = m_root();
		while (x != nullptr) {
			y = x;
			x = m_key_compare(KeyOfValue()(v), m_key(x)) ? m_left(x) : m_right(x);
		}
		return insert(x, y, v);
	}
	pair<iterator, bool> insert_unique(const value_type& v) {
		link_type y = m_header;
		link_type x = m_root();
		bool cmp = true;
		while (x != nullptr) {
			y = x;
			cmp = m_key_compare(KeyOfValue()(v), m_key(x));
			x = cmp ? m_left(x) : m_right(x);
		}
		iterator it(y);
		if (cmp) {
			if (it == begin())
				return pair<iterator, bool>(insert(x, y, v), true);
			else
				--it;
		}
		if (m_key_compare(m_key(it.m_node), KeyOfValue()(v))) {
			return pair<iterator, bool>(insert(x, y, v), true);
		}
		return pair<iterator, bool>(it, false);
	}
	iterator insert_unique(iterator pos, const value_type& v) {
		if (pos.m_node == m_header->m_left) {
			if (size() > 0 && m_key_compare(KeyOfValue()(v), m_key(pos.m_node))) {
				return insert(pos.m_node, pos.m_node, v);
			}
			else
				return insert_unique(v).first;
		}
		else if (pos.m_node == m_header) {
			if (m_key_compare(m_key(m_right_most()), KeyOfValue()(v))) {
				return insert(nullptr, m_right_most(), v);
			}
			else 
				return insert_unique(v).first;
		}
		else {
			iterator before = pos;
			--before;
			if(m_key_compare(m_key(before.m_node), KeyOfValue(v)) &&
				m_key_compare(KeyOfValue(v), m_key(pos.m_node))) {
				if (m_right(before.m_node) == nullptr)
					return insert(nullptr, before.m_node, v);
				else
					return insert(pos.m_node, pos.m_node, v);
			}
			else
				return insert_unique(v).first;
		}
	}

	iterator insert_equal(iterator pos, const value_type& v) {
		if (pos.m_node == m_header->m_left) {
			if (size() > 0 && !m_key_compare(m_key(pos.m_node), KeyOfValue()(v))) {
				return insert(pos.m_node, pos.m_node, v);
			}
			else
				return insert_equal(v);
		}
		else if (pos.m_node == m_header) {
			if (!m_key_compare(KeyOfValue()(v), m_key(m_right_most()))) {
				return insert(nullptr, m_right_most(), v);
			}
			else
				return insert_equal(v);
		}
		else {
			iterator before = pos;
			--before;
			if (!m_key_compare(KeyOfValue(v), m_key(before.m_node)) &&
				!m_key_compare(m_key(pos.m_node), KeyOfValue(v))) {
				if (m_right(before.m_node) == nullptr)
					return insert_equal(nullptr, before.m_node, v);
				else
					return insert_equal(pos.m_node, pos.m_node, v);
			}
			else
				return insert_equal(v);
		}
	}
	// wrapper
	void insert_equal(const value_type* first, const value_type* last) {
		for (; first != last; ++first) {
			insert_equal(*first);
		}
	}
	void insert_equal(const_iterator first, const_iterator last) {
		for (; first != last; ++first) {
			insert_equal(*first);
		}
	}
	void insert_unique(const value_type* first, const value_type* last) {
		for (; first != last; ++first) {
			insert_unique(*first);
		}
	}
	void insert_unique(const_iterator first, const_iterator last) {
		for (; first != last; ++first) {
			insert_unique(*first);
		}
	}

	inline void erase(iterator pos) {
		link_type y = rebalence_for_erase(pos.m_node, 
			                              m_header->m_parent,
			                              m_header->m_left,
			                              m_parent->m_right);
		destroy_node(y);
		--m_node_count;
	}
	void erase(iterator first, iterator last) {
		if (first == begin() && last == end)
			clear();
		else {
			while (first != last) erase(first++);
		}
	}
	void erase(const key_value* first, const key_value* last) {
		while (first != last) erase(*first++);
	}
	inline size_type erase(const key_value& x) {
		pair<iterator, iterator>p = equal_range(x);
		size_type n = 0;
		distance(p.first, p.second, n);
		erase(p.first, p.second);
		return n;
	}
	
	inline void clear() {
		if (m_node_count != 0) {
			erase(m_root());
			m_left_most() = m_header;
			m_right_most() = m_header;
			m_root() = nullptr;
			m_node_count = 0;
		}
	}

	iterator find(const key_value& key) {
		link_type y = m_header;
		link_type x = m_root();
		while (x != nullptr) {
			if (!m_key_compare(m_key(x), key)) {
				y = x;
				x = m_left(x);
			}
			else {
				x = m_right(x);
			}
		}
		iterator it(y);
		return (it == end() || m_key_compare(key, m_key(it.m_node)))
			? end() : it;
	}

	const_iterator find(const key_value& key) const{
		link_type y = m_header;
		link_type x = m_root();
		while (x != nullptr) {
			if (!m_key_compare(m_key(x), key)) {
				y = x;
				x = m_left(x);
			}
			else {
				x = m_right(x);
			}
		}
		const_iterator it(y);
		return (it == end() || m_key_compare(key, m_key(it.m_node)))
			? end() : it;
	}

	inline size_type count(const key_value& key)const {
		pair<const_iterator, const_iterator> p = equal_range(key);
		size_type n = 0;
		distance(p.first, p.second, n);
		return n;
	}

	iterator lower_bound(const key_value& key) {
		link_type y = m_header;
		link_type x = m_root();
		while (x != nullptr) {
			if (!m_key_compare(m_key(x), key)) {
				y = x;
				x = m_left(x);
			}
			else {
				x = m_right(x);
			}
		}
		return iterator(y);
	}

	const_iterator lower_bound(const key_value& key)const {
		link_type y = m_header;
		link_type x = m_root();
		while (x != nullptr) {
			if (!m_key_compare(m_key(x), key)) {
				y = x;
				x = m_left(x);
			}
			else {
				x = m_right(x);
			}
		}
		return const_iterator(y);
	}

	iterator upper_bound(const key_value& key) {
		link_type y = m_header;
		link_type x = m_root();
		while (x != nullptr) {
			if (m_key_compare(key, m_key(x))) {
				y = x;
				x = m_left(x);
			}
			else {
				x = m_right(x);
			}
		}
		return iterator(y);
	}

	const_iterator upper_bound(const key_value& key) const {
		link_type y = m_header;
		link_type x = m_root();
		while (x != nullptr) {
			if (m_key_compare(key, m_key(x))) {
				y = x;
				x = m_left(x);
			}
			else {
				x = m_right(x);
			}
		}
		return const_iterator(y);
	}

	inline pair<iterator, iterator>equal_range(const key_value& key) {
		return pair<iterator, iterator>(lower_bound(key), upper_bound(key));
	}
	inline pair<const_iterator, const_iterator>equal_range(const key_value& key) const{
		return pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key));
	}
	// check the tree is valid [for debugging] ...

private:
	inline void empty_initialize() {
		m_color(m_header) = _RB_RED;
		m_root() = nullptr;
		m_left_most() = m_header;
		m_right_most() = m_header;
	}

	iterator insert(Base_Ptr x, Base_Ptr y, const value_type& v) {
		link_type _x = (link_type)x;
		link_type _y = (link_type)y;
		link_type _z = nullptr;
		if (_y == m_header || _x != nullptr || m_key_compare(KeyOfValue()(v), m_key(_y))) {
			_z = create_node(v);
			m_left(_y) = _z;
			if (_y == m_header) {
				// tree is empty
				m_root() = _z;
				m_right_most() = _z;
			}
			else if (_y == m_left_most()) {
				m_left_most() = _z;
			}
		}
		else {
			_z = create_node(v);
			m_right(_y) = _z;
			if (_y == m_right_most()) {
				m_right_most() = _z;
			}
		}
		m_parent(_z) = _y;
		m_left(_z) = nullptr;
		m_right(_z) = nullptr;
		rebalence(_z, m_header->m_parent);
		++m_node_count;
		return iterator(_z);
	}

	link_type copy(link_type x, link_type p) {
		link_type top = clone_node(x);
		top->m_parent = p;
		if (x->m_right != nullptr) {
			top->m_right = copy(m_right(x), top);
			p = top;
			x = m_left(x);
		}
		while (x != nullptr) {
			link_type y = clone_node(x);
			p->m_left = y;
			y->m_parent = p;
			if (x->m_right != nullptr) {
				y->m_right = copy(m_right(x), y);
			}
			p = y;
			x = m_left(x);
		}
		return top;
	}

	void erase(link_type x) {
		while (x != nullptr) {
			erase(m_right(x));
			link_type y = m_left(x);
			destroy_node(x);
			x = y;
		}
	}

};
// operator
template<typename Key, typename Val, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator==(const RB_Tree<Key, Val, KeyOfValue, Compare, Alloc>&x,
	                   const RB_Tree<Key, Val, KeyOfValue, Compare, Alloc>&y){
	return x.size() == y.size() &&
		equal(x.begin(), x.end(), y.begin(), y.end());}

template<typename Key, typename Val, typename KeyOfValue, typename Compare, typename Alloc>
inline bool operator<(const RB_Tree<Key, Val, KeyOfValue, Compare, Alloc>& x,
	const RB_Tree<Key, Val, KeyOfValue, Compare, Alloc>& y) {
	return lexicalgraphical_compare(x.begin(), x.end(), y.begin(), y.end());}
NAMESPACE_END
#endif // STL_TREE_H