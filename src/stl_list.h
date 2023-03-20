#ifndef STL_LIST_H
#define STL_LIST_H

#include "stl_config.h"
#include "stl_algobase.h"
#include "stl_alloc.h"
#include "stl_iterator.h"
#include "stl_construct.h"

NAMESPACE_START

struct list_node_base {
	struct list_node_base* m_next;
	struct list_node_base* m_prev;
};

template<class Tp>
struct list_node :public list_node_base {
	Tp m_data;
};

struct list_iterator_base {
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef bidirectional_iterator_tag iterator_category;

	list_node_base* m_node;

	list_iterator_base() = default;
	list_iterator_base(list_node_base* x) :m_node(x) {}

	inline void increase() { m_node = m_node->m_next; }

	inline void decrease() { m_node = m_node->m_prev; }

	inline bool operator==(const list_iterator_base& x) const { return m_node == x.m_node; }

	inline bool operator!=(const list_iterator_base& x) const { return m_node != x.m_node; }
};

// list iterator
template<typename Tp, typename Ref,typename Ptr>
struct list_iterator :public list_iterator_base {
	typedef list_iterator<Tp, Tp&, Tp*>             iterator;
	typedef list_iterator<Tp, const Tp&, const Tp*> const_iterator;
	typedef list_iterator<Tp, Ref, Ptr>             Self;

	typedef Tp value_type;
	typedef Ptr pointer;
	typedef Ref reference;
	typedef list_node<Tp> node;

	list_iterator() = default;

	list_iterator(node* x) :list_iterator_base(x) {}

	list_iterator(const iterator& x) :list_iterator_base(x.m_node) {}

	reference operator*()const { return ((node*)m_node)->m_data; }

	pointer operator->()const { return &(operator*()); }

	inline Self& operator++() {
		this->increase();
		return*this;
	}

	inline Self operator++(int) {
		Self tmp = *this;
		this->increase();
		return tmp;
	}

	inline Self& operator--() {
		this->decrease();
		return *this;
	}

	inline Self operator--(int) {
		Self tmp = *this;
		this->decrease();
		return tmp;
	}
};

template<typename Tp, typename Ref, typename Ptr>
inline bidirectional_iterator_tag
iterator_category(const list_iterator<Tp,Ref,Ptr>&) {
	return bidirectional_iterator_tag(); }

template<typename Tp, typename Ref,typename Ptr>
inline Tp* value_type(const list_iterator<Tp, Ref, Ptr>&) {
	return 0; }

inline ptrdiff_t* distance_type(const list_iterator_base&) {
	return 0; }

template<typename Tp, typename Alloc>
class list_base {

public:
	typedef Alloc allocator_type;
	inline allocator_type get_allocator()const {return allocator_type(); }

	list_base(const allocator_type&) {
		m_node = get_node();
		m_node->m_next = m_node;
		m_node->m_prev = m_node;
	}
	~list_base() {
		clear();
		put_node(m_node);
	}

	void clear() {
		list_node<Tp>* cur = (list_node<Tp>*)m_node->m_next;
		while (cur != m_node) {
			list_node<Tp>* tmp = cur;
			cur = (list_node<Tp>*)cur->m_next;
			destroy(&tmp->m_data);
			put_node(tmp);
		}
		m_node->m_next = m_node;
		m_node->m_prev = m_node;
	}

protected:
	typedef SimpleAlloc<list_node<Tp>, Alloc> AllocType;
	list_node<Tp>* get_node() { return AllocType::allocate(); }
	void put_node(list_node<Tp>* p) { AllocType::deallocate(p); }

protected:
	list_node<Tp>* m_node;
};

template<typename Tp, typename Alloc=alloc>
class list :protected list_base<Tp, Alloc> {
	typedef list_base<Tp, Alloc> Base;

public:
	typedef Tp            value_type;
	typedef Tp*           pointer;
	typedef const Tp*     const_pointer;
	typedef Tp&           reference;
	typedef const Tp&     const_reference;
	typedef list_node<Tp> node;
	typedef size_t        size_type;
	typedef ptrdiff_t     difference_type;

	typedef typename Base::allocator_type allocator_type;
	allocator_type get_allocator()const { return Base::get_allocator(); }

public:
	typedef list_iterator<Tp, Tp&, Tp*>             iterator;
	typedef list_iterator<Tp, const Tp&, const Tp*> const_iterator;
	typedef ReverseBidirectionalIter<iterator, value_type, reference, difference_type>
		reverse_iterator;
	typedef ReverseBidirectionalIter<const_iterator, value_type, const reference, difference_type>
		const_reverse_iterator;

protected:
	using Base::get_node;
	using Base::put_node;
	using Base::m_node;

public:
	explicit list(const allocator_type& a = allocator_type()) :Base(a) {}

	list(size_type n, const Tp& value, const allocator_type& a = allocator_type()):Base(a) {
		insert(begin(), n, value); }

	explicit list(size_type n) :Base (allocator_type()){
		insert(begin(), n, Tp());
	}

	list (const Tp* first, const Tp* last, const allocator_type& a = allocator_type()) : Base(a) {
		this->insert(begin(), first, last); }

	list(const_iterator first, const_iterator last, const allocator_type& a= allocator_type()) :Base(a) { 
		this->insert(begin(), first, last); }

	list(const list<Tp, Alloc>& x) :Base(x.get_allocator()) { this->insert(begin(), x.begin(), x.end()); }

	list operator=(const list<Tp, Alloc>& x) {
		if (this == &x) return *this;
		iterator first1 = begin();
		iterator last1 = end();
		const_iterator first2 = x.begin();
		const_iterator last2 = x.end();
		while (first1 != last1 && first2 != last2) {
			*first1++ = *first2++; }
		if (first2 == last2) erase(first1, last1);
		else insert(last1, first2, last2);
		return *this;
	}

	~list() = default;

	inline node* create_node(const Tp& x) {
		node* p = get_node();
		Construct(&p->m_data, x);
		return p;
	}
	inline node* create_node() {
		node* p = get_node();
		Construct(p->m_data);
		return p;
	}
	inline iterator begin() { return (node*)(m_node->m_next); }
	inline const_iterator begin()const { return(node*) m_node->m_next; }
	
	inline iterator end() { return m_node; }
	inline const_iterator end() const { return m_node; }
	
	inline reverse_iterator rbegin() { return reverse_iterator(end()); }
	inline const_reverse_iterator rbegin()const { return const_reverse_iterator(end()); }

	inline reverse_iterator rend() { return reverse_iterator(begin()); }
	inline const_reverse_iterator rend()const { return const_reverse_iterator(begin()); }

	inline bool empty()const { return m_node->m_next == m_node; }

	inline size_type size()const {
		size_type result = 0;
		distance(begin(), end(), result);
		return result;
	}

	inline size_type max_size()const { return size_type(-1); }

	inline reference front() { return *begin(); }
	inline const_reference front() const { return *begin(); }
	inline reference back() { return *(--end()); }
	inline reference back()const { return *(--end()); }

	inline void swap(list<Tp, Alloc>& x) { stl::swap(m_node, x.m_node); }

	inline iterator insert(iterator position, const Tp& x) {
		node* tmp = create_node(x);
		tmp->m_next = position.m_node;
		tmp->m_prev = position.m_node->m_prev;
		position.m_node->m_prev->m_next = tmp;
		position.m_node->m_prev = tmp;
		return tmp;
	}

	inline iterator insert(iterator position) { return insert(position, Tp()); }

	void insert(iterator pos, const_iterator first, const_iterator last) {
		for (; first != last; ++first) {
			insert(pos, *first);
		}
	}

	void insert(iterator pos, const Tp* first, const Tp* last) {
		for (; first != last; ++first) {
			insert(pos, *first);
		}
	}

	inline void insert(iterator pos, size_type n, const Tp& x) {
		return fill_insert(pos, n, x);}

	inline void push_front(const Tp& x) { insert(begin(), x); }

	inline void push_front() { insert(begin()); }

	inline void push_back(const Tp&x) {insert(end(),x); }

	inline void push_back() { insert(end()); }

	inline iterator erase(iterator pos) {
		list_node_base* next_node = pos.m_node->m_next;
		list_node_base* prev_node = pos.m_node->m_prev;
		node* n = (node*)pos.m_node;
		prev_node->m_next = next_node;
		next_node->m_prev = prev_node;
		Destroy(&n->m_data);
		put_node(n);
		return iterator((node*)next_node);
	}

	iterator erase(iterator first, iterator last) {
		while (first != last) {
			erase(first++);
		}
		return last;
	}

	inline void clear() { Base::clear(); }

	void resize(size_type new_size, const Tp& x) {
		iterator it = begin();
		size_type len = 0;
		for (; it != end() && len < new_size; ++it, ++len);
		// if new_size <= actual size
		if (len == new_size) erase(it, end());
		// if new size > actual size
		else insert(end(), new_size - len, x);
	}

	inline void resize(size_type new_size) { this->resize(new_size, Tp()); }

	inline void pop_front() { erase(begin()); }

	inline void pop_back() { erase(--end()); }

protected:
	void fill_insert(iterator pos, size_type n, const Tp & x) {
		for (; n > 0; --n) {
			insert(pos, x);
		}
	}
	void fill_assign(size_type n, const Tp&val){
		iterator it = begin();
		for (; it != end() && n > 0; ++it, --n) { *it = val; }
		if (n > 0) insert(end(), n, val);
		else erase(it, end());
	}

public:
	inline void assign(size_type n, const Tp& value) { return fill_assign(n, value); }
	
protected:
	inline void transfer(iterator pos, iterator first, iterator last) {
		// Ctrl+X the [first, last) in front of the pos
		// Remove [first, last)
		if (pos == last) return;
		last.m_node->m_prev->m_next = pos.m_node;
		first.m_node->m_prev->m_next = last.m_node;
		pos.m_node->m_prev->m_next = first.m_node;
		// splice [first, last) into its new position
		list_node_base* tmp = pos.m_node->m_prev;
		pos.m_node->m_prev = last.m_node->m_prev;
		last.m_node->m_prev = first.m_node->m_prev;
		first.m_node->m_prev = tmp;
	}
public:
	inline void splice(iterator pos, list& x) {
		// Ctrl + X the x in front of the pos
		if (!x.empty()) {
			this->transfer(pos, x.begin(), x.end());
		}
	}

	inline void splice(iterator pos, iterator it) {
		iterator j = it;
		++j;
		if (pos == it || pos == j) return;
		this->transfer(pos, it, j);
	}

	inline void splice(iterator pos, iterator first, iterator last) {
		if (first != last) this->transfer(pos, first, last);
	}

	void remove(const Tp& value) {
		iterator first = begin();
		iterator last = end();
		while (first != last) {
			iterator next = first;
			++next;
			if (*first == value) erase(first);
			first = next;
		}
	}

	void unique() {
		iterator first = begin();
		iterator last = end();
		if (first == last) return;
		iterator next = first;
		while (++next != last) {
			if (*first == *next) erase(next);
			else first = next;
			next = first;
		}
	}

	void merge(list& x) {
		iterator first1 = begin();
		iterator last1 = end();
		iterator first2 = x.begin();
		iterator last2 = x.end();
		while (first1 != last1 && first2 != last2) {
			// must override operator<()
			if (*first2 < *first1) {
				iterator next = first2;
				transfer(first1, first2, ++next);
				first2 = next;
			}
			else {
				++first1;
			}
		}
		if (first2 != last2) transfer(last1, first2, last2);
	}

	void reverse() {
		list_node_base* tmp = m_node;
		do {
			stl::swap(tmp->m_next, tmp->m_prev);
			tmp = tmp->m_prev;
		} while (tmp != m_node);
	}

	void sort() {
		if (m_node->m_next == m_node || m_node->m_next->m_next == m_node)return;
		list carry;
		list counter[64];
		int fill = 0;
		while (!empty()) {
			int i = 0;
			carry.splice(carry.begin(), begin());
			while (i < fill && !counter[i].empty()) {
				counter[i].merge(carry);
				carry.swap(counter[i++]);
			}
			carry.swap(counter[i]);
			if (i == fill) ++fill;
		}
		for (int i = 1; i < fill; ++i) {
			counter[i].merge(counter[i - 1]);
		}
		// assert fill >= 1
		this->swap(counter[fill - 1]);
	}
};
// override operator XXX
template<typename Tp, typename Alloc>
inline bool operator==(const list<Tp, Alloc>& x, const list<Tp, Alloc>& y) {
	typedef typename list<Tp, Alloc>::const_iterator const_iterator;
	const_iterator end1 = x.end();
	const_iterator end2 = y.end();
	const_iterator start1 = x.begin();
	const_iterator start2 = y.begin();
	while (start1 != end1 && start2 != end2 && (*start1 == *start2)) {
		++start1;
		++start2;
	}
	return start1 == end1 && start2 == end2;
}

NAMESPACE_END
#endif