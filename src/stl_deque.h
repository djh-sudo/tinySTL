#ifndef STL_DEQUE_H
#define STL_DEQUE_H

#include "stl_config.h"
#include "stl_iterator.h"
#include "stl_algobase.h"
#include "stl_alloc.h"
#include "stl_vector.h"

NAMESPACE_START

inline const size_t buf_size(size_t size) {
	return size < 512 ? (size_t)(512 / size) : 1;
}
// iterator
template<typename Tp, typename Ref,typename Ptr>
struct deque_iterator {
	static size_t buffer_size() { return buf_size(sizeof(Tp)); }
	typedef deque_iterator<Tp, Tp&, Tp*>             iterator;
	typedef deque_iterator<Tp, const Tp&, const Tp*> const_iterator;
	typedef random_access_iterator_tag   iterator_category;
	typedef Tp                           value_type;
	typedef Ptr                          pointer;
	typedef Ref                          reference;
	typedef size_t                       size_type;
	typedef ptrdiff_t                    difference_type;
	typedef Tp**                         map_pointer;
	typedef deque_iterator<Tp, Ref, Ptr> Self;
	// data
	Tp* m_cur;
	Tp* m_first;
	Tp* m_last;
	map_pointer m_node;
	deque_iterator(Tp*x, map_pointer y):m_cur(x),m_first(*y), 
		                                m_last(*y + buffer_size()),m_node(y) {}
	deque_iterator() :m_node(0), m_cur(0), m_first(0), m_last(0) {}
	deque_iterator(const iterator& x) :m_cur(x.m_cur), m_first(x.m_first), 
		                               m_last(x.m_last), m_node(x.m_node) {}
	// override
	inline reference operator*()const { return *m_cur; }
	inline pointer operator->()const { return m_cur; }
	inline difference_type operator-(const Self&x)const{
		return difference_type(buffer_size()) * (m_node - x.m_node - 1) 
		       + (m_cur - m_first) + (x.m_last - x.m_cur);
	}
	// 
	Self& operator++() {
		++m_cur;
		if (m_cur == m_last) {
			set_node(m_node + 1);
			m_cur = m_first;
		}
		return *this;
	}
	Self operator++(int) {
		Self tmp = *this;
		++* this;
		return tmp;
	}
	Self& operator--() {
		if (m_cur == m_first) {
			set_node(m_node - 1);
			m_cur = m_last;
		}
		--m_cur;
		return *this;
	}
	Self operator--(int) {
		Self tmp = *this;
		--*this;
		return tmp;
	}
	Self& operator+=(difference_type n) {
		difference_type offset = n + (m_cur - m_first);
		if (offset >= 0 && offset < difference_type(buffer_size())) {
			m_cur += n;
		}
		else {
			difference_type node_off = offset > 0 ? 
				            offset / difference_type(buffer_size()):
				            -difference_type((-offset - 1) / buffer_size()) - 1;
			set_node(m_node + node_off);
			m_cur = m_first + (offset - node_off * difference_type(buffer_size()));
		}
		return*this;
	}
	inline Self operator+=(difference_type n) const{
		Self tmp = *this;
		return tmp += n;
	}
	inline Self operator+(difference_type n)const {
		Self tmp = *this;
		return tmp += n;
	}
	inline Self& operator-=(difference_type n) {
		return *this += -n;
	}
	inline Self operator-(difference_type n)const {
		Self tmp = *this;
		return tmp -= n;
	}
	inline reference operator[](difference_type n) const{
		return *(*this + n);
	}
	inline bool operator==(const Self& x) { return m_cur == x.m_cur; }
	inline bool operator!=(const Self& x) { return !(*this == x); }
	bool operator<(const Self&x)const{
		return (m_node == x.m_node) ? 
			   (m_cur < x.m_cur) : (m_node < x.m_node);
	}
	inline bool operator>(const Self& x)const { return x < *this; }
	inline bool operator<=(const Self& x)const { return !(*this > x); }
	inline bool operator>=(const Self& x)const { return !(*this < x); }
	
	// auxiliary
	void set_node(map_pointer new_node) {
		m_node = new_node;
		m_first = *new_node;
		m_last = m_first + difference_type(buffer_size());
	}
};
// override
template<typename Tp, typename Ref, typename Ptr>
inline deque_iterator<Tp, Ref, Ptr>
operator+(ptrdiff_t n, const deque_iterator<Tp, Ref, Ptr>& x) {
	return x + n;
}
template<typename Tp, typename Ref, typename Ptr>
inline random_access_iterator_tag
iterator_category(const deque_iterator<Tp, Ref, Ptr>&) {
	return random_access_iterator_tag();}
template<typename Tp, typename Ref, typename Ptr>
inline Tp* value_type(const deque_iterator<Tp, Ref, Ptr>&) { return 0; }

template<typename Tp, typename Ref, typename Ptr>
inline ptrdiff_t* distance_type(const deque_iterator<Tp, Ref, Ptr>&) { return 0; }

// deque_base
template<typename Tp, typename Alloc>
class deque_base {

public:
	typedef deque_iterator<Tp, Tp&, Tp*>             iterator;
	typedef deque_iterator<Tp, const Tp&, const Tp*> const_iterator;
	typedef Alloc allocator_type;

	allocator_type get_allocator()const { return allocator_type(); }

	explicit deque_base(size_t number):m_map(0), m_map_size(0), m_start(),m_finish() {
		initialize_map(number);
	}
	deque_base() :m_map(0), m_map_size(0), m_start(), m_finish() {}
	~deque_base() {
		if (m_map != nullptr) {
			destroy_node(m_start.m_node, m_finish.m_node + 1);
			deallocate_map(m_map, m_map_size);
			m_map = nullptr;
		}
	}
protected:
	enum { INITIAL_MAP_SIZE = 8 };

	void initialize_map(size_t num_elements) {
		size_t num_nodes = num_elements / buf_size(sizeof(Tp)) + 1;
		m_map_size = max((size_t)INITIAL_MAP_SIZE, num_nodes + 2);
		// allocate memory
		m_map = allocate_map(m_map_size);
		Tp** start = m_map + (m_map_size - num_nodes) / 2;
		Tp** finish = start + num_nodes;
		create_node(start, finish);
		m_start.set_node(start);
		m_finish.set_node(finish - 1);
		m_start.m_cur = m_start.m_first;
		m_finish.m_cur = m_finish.m_first + num_elements % buf_size(sizeof(Tp));
	}
	void create_node(Tp** start, Tp** finish) {
		Tp** cur = nullptr;
		for (cur = start; cur < finish; ++cur) {
			*cur = allocate_node();
		}
	}
	void destroy_node(Tp** start, Tp** finish) {
		for (Tp** n = start; n < finish; ++n) {
			deallocate_node(*n);
		}
	}

	typedef SimpleAlloc<Tp, Alloc> node_alloc_type;
	typedef SimpleAlloc<Tp*, Alloc> map_alloc_type;

	inline Tp* allocate_node() const { return node_alloc_type::allocate(buf_size(sizeof(Tp))); }
	inline void deallocate_node(Tp* p)const { node_alloc_type::deallocate(p, buf_size(sizeof(Tp))); }
	inline Tp** allocate_map(size_t n) { return map_alloc_type::allocate(n); }
	inline void deallocate_map(Tp** p, size_t n) { map_alloc_type::deallocate(p, n); }

protected:
	Tp** m_map;
	size_t m_map_size;
	iterator m_start;
	iterator m_finish;

};
// deque
template<typename Tp, typename Alloc=alloc>
class deque :protected deque_base<Tp, Alloc> {

private:
	typedef deque_base<Tp, Alloc> Base;
public:
	typedef Tp        value_type;
	typedef Tp*       pointer;
	typedef const Tp* const_pointer;
	typedef Tp&       reference;
	typedef const Tp& const_reference;
	typedef size_t    size_type;
	typedef ptrdiff_t difference_type;

	typedef typename Base::allocator_type allocator_type;
public:
	typedef typename Base::iterator       iterator;
	typedef typename Base::const_iterator const_iterator;
	typedef ReverseIter<iterator, value_type, reference, difference_type> reverse_iterator;
	typedef ReverseIter<const_iterator, 
		    value_type, const_reference, difference_type> const_reverse_iterator;
protected:
	typedef pointer* map_pointer;
	static size_t buffer_size() { return buf_size(sizeof(Tp)); }

protected:
	using Base::m_start;
	using Base::m_finish;
	using Base::m_map_size;
	using Base::m_map;
	using Base::allocate_node;
	using Base::deallocate_node;
	using Base::allocate_map;
	using Base::deallocate_map;

public:
	deque() :Base(0) {}
	explicit deque(const deque& x) :Base(x.size()) {
		uninitialized_copy(x.begin(), x.end(), m_start);}
	deque(size_type n, const value_type& val):Base(n) {
		fill_initialize(val);}
	explicit deque(size_type n) :Base(n) {
		fill_initialize(value_type());}
	~deque() { destroy(m_start, m_finish); }
	
	deque& operator=(const deque& x) {
		if (&x == this) return*this;
		const size_type length = size();
		if (length > x.size()) {
			erase(copy(x.begin(), x.end(), m_start), m_finish);
		}
		else {
			const iterator mid = x.begin() + difference_type(length);
			copy(x.begin(), x.end(), m_start);
			insert(m_finish, mid, x.end());
		}
	}
	inline void swap(deque& x) {
		std::swap(m_start, x.m_start);
		std::swap(m_finish, x.m_finish);
		std::swap(m_map, x.m_map);
		std::swap(m_map_size, m_map_size);
	}

	inline iterator begin() { return m_start; }
	inline iterator begin()const { return m_start; }
	inline iterator end() { return m_finish; }
	inline iterator end()const { return m_finish; }
	inline reverse_iterator rbegin() { return reverse_iterator(m_finish); }
	inline reverse_iterator rend() { return reverse_iterator(m_start); }
	inline const_reverse_iterator rbegin()const { return const_reverse_iterator(m_finish); }
	inline const_reverse_iterator rend()const { return const_reverse_iterator(m_start); }
	// override
	inline reference operator[](size_type n) { return m_start[difference_type(n)]; }
	inline const_reference operator[](size_type n)const { return m_start[difference_type(n)]; }

	inline reference front() { return *m_start; }
	inline reference back() {
		iterator tmp = m_finish;
		--tmp;
		return*tmp;
	}
	inline const_reference front()const { return *m_start; }
	inline const_reference back()const {
		const_iterator tmp = m_finish;
		--tmp;
		return*tmp;
	}
	inline size_type size()const { return (size_type)(m_finish - m_start); }
	inline size_type max_size()const { return size_type(-1); }
	inline bool empty()const { return m_finish == m_start; }
	
	void push_back(const value_type& x) {
		if (m_finish.m_cur != m_finish.m_last - 1) {
			construct(m_finish.m_cur, x);
			++m_finish.m_cur;
		}
		else {
			push_back_aux(x);
		}
	}
	void push_back() {
		if (m_finish.m_cur != m_finish.m_last - 1) {
			construct(m_finish.m_cur);
			++m_finish.m_cur;
		}
		else {
			push_back_aux();
		}
	}
	void push_front(const value_type& x) {
		if (m_start.m_cur != m_start.m_first) {
			construct(m_start.m_cur - 1, x);
			--m_start.m_cur;
		}
		else {
			push_front_aux(x);
		}
	}
	void push_front() {
		if (m_start.m_cur != m_start.m_first) {
			construct(m_start.m_cur - 1);
			--m_start.m_cur;
		}
		else {
			push_front_aux();
		}
	}
	void pop_back() {
		if (m_finish.m_cur != m_finish.m_first) {
			--m_finish.m_cur;
			destroy(m_finish.m_cur);
		}
		else {
			pop_back_aux();
		}
	}
	void pop_front() {
		if (m_start.m_cur != m_start.m_last) {
			destroy(m_start.m_cur);
			++m_start.m_cur;
		}
		else {
			pop_front_aux();
		}
	}

	void insert(iterator pos, const iterator first, const iterator last) {
		size_type n = last - first;
		if (pos.m_cur == m_start.m_cur) {
			iterator new_start = reserve_ele_at_front(n);
			uninitialized_copy(first, last, new_start);
			m_start = new_start;
		}
		else if (pos.m_cur == m_finish.m_cur) {
			iterator new_finish = reserve_ele_at_back(n);
			uninitialized_copy(first, last, m_finish);
			m_finish = new_finish;
		}
		else {
			insert_aux(pos, first, last, n);
		}
	}

	// special
	void insert(iterator pos, const value_type* first, const value_type* last) {
		size_type n = last - first;
		if (pos.m_cur == m_start.m_cur) {
			iterator new_start = reserve_ele_at_front(n);
			uninitialized_copy(first, last, new_start);
			m_start = new_start;
		}
		else if (pos.m_cur == m_finish.m_cur) {
			iterator new_finish = reserve_ele_at_back(n);
			uninitialized_copy(first, last, m_finish);
			m_finish = new_finish;
		}
		else {
			insert_aux(pos, first, last, n);
		}
	}

	iterator erase(iterator pos) {
		iterator next = pos;
		++next;
		difference_type index = pos - m_start;
		const size_type length = size();
		if (size_type(index) < length / 2) {
			copy_backward(m_start, pos, next);
			pop_front();
		}
		else {
			copy(next, m_finish, pos);
			pop_back();
		}
		return m_start + index;
	}

	iterator erase(iterator first, iterator last) {
		if (first == m_start && last == m_finish) {
			clear();
			return m_finish;
		}
		else {
			difference_type n = last - first;
			difference_type ele_before = first - m_start;
			if (ele_before < difference_type((size() - n) / 2)) {
				copy_backward(m_start, first, last);
				iterator new_start = m_start + n;
				destroy(m_start, new_start);
				destroy_node(m_start.m_node, new_start.m_node);
				m_start = new_start;
			}
			else {
				copy(last, m_finish, first);
				iterator new_finish = m_finish - n;
				destroy(new_finish, m_finish);
				destroy_node(new_finish.m_node + 1, m_finish.m_node + 1);
				m_finish = new_finish;
			}
			return m_start + ele_before;
		}
	}

	void clear() {
		for (map_pointer node = m_start.m_node + 1; node < m_finish.m_node; ++node) {
			destroy(*node, *node + buffer_size());
			deallocate_node(*node);
		}
		if (m_start.m_node != m_finish.m_node) {
			destroy(m_start.m_cur, m_start.m_last);
			destroy(m_finish.m_first, m_finish.m_cur);
			deallocate_node(m_finish.m_first);
		}
		else {
			destroy(m_start.m_cur, m_finish.m_cur);
		}
		m_finish = m_start;
	}

public:
	iterator insert(iterator pos, const value_type& x) {
		if (pos.m_cur == m_start.m_cur) {
			push_front(x);
			return m_start;
		}
		else if (pos.m_cur == m_finish.m_cur) {
			push_back(x);
			iterator tmp = m_finish;
			--tmp;
			return tmp;
		}
		else {
			return insert_aux(pos, x);
		}
	}
	inline iterator insert(iterator pos) {
		return insert(pos, value_type());
	}
	inline void insert(iterator pos, size_type n, const value_type& x) {
		fill_insert(pos, n, x);
	}

protected:
	void fill_initialize(const value_type& value){
		for (map_pointer cur = m_start.m_node; cur < m_finish.m_node; ++cur) {
			uninitialized_fill(*cur, *cur + buffer_size(), value);
		}
		uninitialized_fill(m_finish.first, m_finish.cur, value);
	}
	
	inline void push_back_aux(const value_type& x) {
		reserve_map_at_back();
		*(m_finish.m_node + 1) = allocate_node();
		construct(m_finish.m_cur, x);
		m_finish.set_node(m_finish.m_node + 1);
		m_finish.m_cur = m_finish.m_first;
	}
	inline void push_back_aux() {
		reserve_map_at_back();
		*(m_finish.m_node + 1) = allocate_node();
		construct(m_finish.m_cur);
		m_finish.set_node(m_finish.m_node + 1);
		m_finish.m_cur = m_finish.m_first;
	}
	inline void push_front_aux(const value_type&x) {
		reserve_map_at_front();
		*(m_start.m_node - 1) = allocate_node();
		m_start.set_node(m_start.m_node - 1);
		m_start.m_cur = m_start.m_last - 1;
		construct(m_start.m_cur, x);
	}
	inline void push_front_aux() {
		reserve_map_at_front();
		*(m_start.m_node - 1) = allocate_node();
		m_start.set_node(m_start.m_node - 1);
		m_start.m_cur = m_start.m_last - 1;
		construct(m_start.m_cur);
	}
	
	inline void pop_back_aux() {
		deallocate_node(m_finish.m_first);
		m_finish.set_node(m_finish.m_node - 1);
		m_finish.m_cur = m_finish.m_last - 1;
		destroy(m_finish.m_cur);
	}
	inline void pop_front_aux() {
		destroy(m_start.m_cur);
		deallocate_node(m_start.m_first);
		m_start.set_node(m_start.m_node + 1);
		m_start.m_cur = m_start.m_first;
	}
	
	
	inline void reserve_map_at_back(const size_type& add = 1) {
		if (add + 1 > m_map_size - (m_finish.m_node - m_map)) {
			reallocate_map(add, false);
		}
	}
	
	inline void reserve_map_at_front(const size_type add = 1) {
		if (add > size_type(m_start.m_node - m_map)) {
			reallocate_map(add, true);
		}
	}
	
	iterator reserve_ele_at_front(size_type n) {
		size_type vacancy = m_start.m_cur - m_start.m_first;
		if (n > vacancy) {
			new_ele_at_front(n - vacancy);
		}
		return m_start - difference_type(n);
	}
	
	iterator reserve_ele_at_back(size_type n) {
		size_type vacancy = (m_finish.m_last - m_finish.m_cur) - 1;
		if (n > vacancy) {
			new_ele_at_back(n - vacancy);
		}
		return m_finish + difference_type(n);
	}

	void fill_insert(iterator pos, size_type n, const value_type& value) {
		if (pos.m_cur == m_start.m_cur) {
			iterator new_start = reserve_ele_at_front(n);
			uninitialized_fill(new_start, m_start, value);
			m_start = new_start;
		}
		else if (pos.m_cur == m_finish.m_cur) {
			iterator new_finish = reserve_ele_at_back(n);
			uninitialized_fill(m_finish, new_finish, value);
			m_finish = new_finish;
		}
		else {
			insert_aux(pos, n, value);
		}
	}
	
	void insert_aux(iterator pos, const iterator first, const iterator last, size_type n) {
		const difference_type ele_before = pos - m_start;
		size_type length = size();
		if (ele_before < length / 2) {
			// insert in front of the deque
			iterator new_start = reserve_ele_at_front(n);
			// [WARNING] `m_start` may reset after calling `reserve_ele_at_front`
			pos = m_start + ele_before;
			iterator old_start = m_start;
			if (ele_before >= n) {
				iterator start_n = m_start + n;
				uninitialized_copy(m_start, start_n, new_start);
				m_start = new_start;
				copy(start_n, pos, old_start);
				copy(first, last, pos - difference_type(n));
			}
			else {
				const_iterator mid = first + (n - ele_before);
				iterator it = uninitialized_copy(m_start, pos, new_start);
				uninitialized_copy(first, mid, it);
				m_start = new_start;
				m_start = new_start;
				copy(mid, last, old_start);
			}
		}
		else {
			// insert in back of deque
			iterator new_finish = reserve_ele_at_back(n);
			iterator old_finish = m_finish;
			const difference_type ele_after = length - ele_before;
			// this is important
			pos = m_finish - ele_after;
			if (ele_after > n) {
				iterator finish_n = m_finish - difference_type(n);
				uninitialized_copy(finish_n, m_finish, m_finish);
				m_finish = new_finish;
				copy_backward(pos, finish_n, old_finish);
				copy(first, last, pos);
			}
			else {
				const iterator mid = first + ele_after;
				iterator it = uninitialized_copy(mid, last, m_finish);
				uninitialized_copy(pos, m_finish, it);
				m_finish = new_finish;
				copy(first, mid, pos);
			}
		}
	}
	
	void insert_aux(iterator pos, size_type n, const value_type& x) {
		const difference_type ele_before = pos - m_start;
		size_type length = size();
		if (ele_before < difference_type(length / 2)) {
			iterator new_start = reserve_ele_at_front(n);
			iterator old_start = m_start;
			// [WARNING] `m_start` may reset after calling `reserve_ele_at_front`
			pos = m_start + ele_before;
			if (ele_before >= difference_type(n)) {
				iterator start_n = m_start + difference_type(n);
				uninitialized_copy(m_start, start_n, new_start);
				m_start = new_start;
				copy(start_n, pos, old_start);
				fill(pos - difference_type(n), pos, x);
			}
			else {
				iterator it = uninitialized_copy(m_start, pos, new_start);
				uninitialized_fill(it, m_start, x);
				m_start = new_start;
				fill(old_start, pos, x);
			}
		}
		else {
			iterator new_finish = reserve_ele_at_back(n);
			iterator old_finish = m_finish;
			const difference_type ele_after = difference_type(length) - ele_before;
			pos = m_finish - ele_after;
			if (ele_after > difference_type(n)) {
				iterator finish_n = m_finish - difference_type(n);
				uninitialized_copy(finish_n, m_finish, m_finish);
				m_finish = new_finish;
				copy_backward(pos, finish_n, old_finish);
				fill(pos, pos + difference_type(n), x);
			}else{
				iterator it = fill(m_finish, pos + n, x);
				uninitialized_copy(pos, m_finish, it);
				m_finish = new_finish;
				fill(pos, old_finish, x);
			}
		}
	}
	
	void insert_aux(iterator pos, const value_type* first, const value_type* last, size_type n) {
		const difference_type ele_before = pos - m_start;
		const size_type length = size();
		if (ele_before < length / 2) {
			iterator new_start = reserve_ele_at_front(n);
			iterator old_start = m_start;
			// [WARNING] `m_start` may reset after calling `reserve_ele_at_front`
			pos = m_start + ele_before;
			if (ele_before >= difference_type(n)) {
				iterator start_n = m_start + n;
				uninitialized_copy(m_start, start_n, new_start);
				m_start = new_start;
				copy(start_n, pos, old_start);
				copy(first, last, pos - difference_type(n));
			}
			else {
				const value_type* mid = first + difference_type(n) - ele_before;
				iterator it = uninitialized_copy(m_start, pos, new_start);
				uninitialized_copy(first, mid, it);
				m_start = new_start;
				copy(mid, last, old_start);
			}
		}
		else {
			iterator new_finish = reserve_ele_at_back(n);
			iterator old_finish = m_finish;
			const difference_type ele_after = difference_type(length) - ele_before;
			pos = m_finish - ele_after;
			if (ele_after > difference_type(n)) {
				iterator finish_n = m_finish - difference_type(n);
				uninitialized_copy(finish_n, m_finish, m_finish);
				m_finish = new_finish;
				copy_backward(pos, finish_n, old_finish);
				copy(first, last, pos);
			}
			else {
				const value_type* mid = first + ele_after;
				iterator it = uninitialized_copy(mid, last, m_finish);
				uninitialized_copy(pos, m_finish, it);
				m_finish = new_finish;
				copy(first, mid, pos);
			}
		}
	}

	iterator insert_aux(iterator pos, const value_type& x) {
		difference_type index = pos - m_start;
		if (size_type(index) < (size() / 2)) {
			push_front(front());
			iterator f1 = m_start;
			++f1;
			iterator f2 = f1;
			++f2;
			pos = m_start + index;
			iterator p1 = pos;
			++p1;
			copy(f2, p1, f1);
		}
		else {
			push_back(back());
			iterator b1 = m_finish;
			--b1;
			iterator b2 = b1;
			--b2;
			pos = m_start + index;
			copy_backward(pos, b2, b1);
		}
		*pos = x;
		return pos;
	}

	void reallocate_map(size_type adds, bool add_front) {
		size_type old_nodes_nums = m_finish.m_node - m_start.m_node + 1;
		size_type new_nodes_nums = old_nodes_nums + adds;
		map_pointer new_starts;
		if (m_map_size > 2 * new_nodes_nums) {
			// just move
			new_starts = m_map + (m_map_size - new_nodes_nums) / 2 +
				         (add_front ? adds : 0);
			if (new_starts < m_start.m_node) {
				copy(m_start.m_node, m_finish.m_node + 1, new_starts);
			}
			else {
				copy_backward(m_start.m_node, m_finish.m_node + 1, 
					          new_starts + old_nodes_nums);
			}
		}
		else {
			// allocate new memory
			size_type new_map_size = m_map_size + max(m_map_size, adds) + 2;
			map_pointer new_map = allocate_map(new_map_size);
			new_starts = new_map + (new_map_size - new_nodes_nums) / 2 +
				(add_front ? adds : 0);
			copy(m_start.m_node, m_finish.m_node + 1, new_starts);
			deallocate_map(m_map, m_map_size);
			m_map = new_map;
			m_map_size = new_map_size;
		}
		m_start.set_node(new_starts);
		m_finish.set_node(new_starts + old_nodes_nums - 1);
	}

	void new_ele_at_front(size_type new_ele) {
		size_type new_nodes = (new_ele + buffer_size() - 1) / buffer_size();
		reserve_map_at_front(new_nodes);
		for (size_type i = 1; i <= new_nodes; ++i) {
			*(m_start.m_node - i) = allocate_node();
		}
	}
	void new_ele_at_back(size_type new_ele) {
		size_type new_nodes = (new_ele + buffer_size() - 1) / buffer_size();
		reserve_map_at_back(new_nodes);
		for (size_type i = 1; i <= new_nodes; ++i) {
			*(m_finish + i) = allocate_node();
		}
	}

};
// override operator
template<typename Tp, typename Alloc>
inline bool operator==(const deque<Tp, Alloc>& x,
	                   const deque<Tp, Alloc>& y) {
	return x.size() == y.size() && 
		   equal(x.begin(), x.end(), y.begin(), y.end());
}

template<typename Tp, typename Alloc>
inline bool operator<(const deque<Tp, Alloc>& x,
	                  const deque<Tp, Alloc>& y) {
	return lexigraphical_compare(x.begin(), x.end(), y.begin(), y.end());
}

NAMESPACE_END
#endif	