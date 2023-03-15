#ifndef STL_VECTOR_H
#define STL_VECTOR_H
#include "stl_config.h"
#include "stl_alloc.h"
#include "stl_uninitialized.h"
#include "stl_algobase.h"

NAMESPACE_START

template<typename Tp, typename Alloc>
class vector_base {

public:
	typedef Alloc allocator_type;
	inline allocator_type get_allocator()const { return allocator_type(); }
	vector_base(const Alloc&):m_start(nullptr), m_finish(nullptr), m_end_of_storage(nullptr) {}
	vector_base(size_t n, const Alloc&) :m_start(nullptr), m_finish(nullptr), m_end_of_storage(nullptr) {
		m_start = m_allocator(n);
		m_finish = m_start;
		m_end_of_storage = m_start + n;
	}
	~vector_base() { m_deallocator(m_start, size_t(m_end_of_storage - m_start)); }

protected:
	typedef SimpleAlloc<Tp, Alloc> m_data_allocator;
	Tp* m_allocator(size_t n) {
		return m_data_allocator::allocate(n);}
	inline void m_deallocator(Tp* p, size_t n) {
		m_data_allocator::deallocate(p, n);}

protected:
	Tp* m_start;
	Tp* m_finish;
	Tp* m_end_of_storage;
};

template<typename Tp, typename Alloc=alloc>
class vector :protected vector_base<Tp, Alloc> {
private:
	typedef vector_base<Tp, Alloc> Base;
	using Base::m_start;
	using Base::m_finish;
	using Base::m_end_of_storage;
	using Base::m_allocator;
	using Base::m_deallocator;

public:
	typedef Tp value_type;
	typedef Tp* pointer;
	typedef const Tp* const_pointer;
	typedef Tp* iterator;
	typedef const Tp* const_iterator;
	typedef Tp& reference;
	typedef const Tp& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef Base::allocator_type allocator_type;

	inline allocator_type get_allocator()const { return Base::get_allocator(); }

	typedef ReverseIter<iterator, value_type, reference, difference_type> reverse_iterator;
	typedef ReverseIter<const_iterator, value_type, const_reference, difference_type> const_reverse_iterator;

public:
	explicit vector(const allocator_type& a = allocator_type()) :Base(a) {}
	vector(size_type n, const Tp& value, const allocator_type& a = allocator_type()) :Base(n, a) {
		m_finish = uninitialized_fill_n(m_start, n, value);
	}
	explicit vector(size_type n) :Base(n, allocator_type()) {
		m_finish = uninitialized_fill_n(m_start, n, Tp());
	}
	vector(const vector<Tp, Alloc>& x):Base(x.size(), x.get_allocator()) {
		m_finish = uninitialized_copy(x.begin(), x.end(), m_start);
	}
	vector(const Tp* first, const Tp* last, const allocator_type& a = allocator_type()) :Base(last - first, a) {
		m_finish = uninitialized_copy(first, last, m_start);
	}

	~vector() { 
		destroy(m_start, m_finish); }

	inline iterator begin() { return m_start; }
	inline iterator begin()const { return m_start; }
	inline iterator end() { return m_finish; }
	inline iterator end()const { return m_finish; }
	inline reverse_iterator rbegin() { return reverse_iterator(end()); }
	inline const_reverse_iterator rbegin()const { return const_reverse_iterator(end()); }
	inline reverse_iterator rend() { return reverse_iterator(begin()); }
	inline const_reverse_iterator rend()const { return const_reverse_iterator(begin()); }

	inline size_type size()const { return size_type(end() - begin()); }
	inline size_type max_size()const { return size_type(-1) / sizeof(Tp); }
	inline size_type capacity()const { return size_type(m_end_of_storage - m_start); }
	inline bool empty()const { return begin() == end(); }
	inline reference front() { return*begin(); }
	inline const_reference front()const { return *begin(); }
	inline reference back() { return *end(); }
	inline const_reference back()const { return*end(); }

	inline void push_back(const Tp& x) {
		if (m_finish != m_end_of_storage) {
			construct(m_finish, x);
			++m_finish;
		}
		else {
			insert_aux(end(), x);
		}
	}
	inline void push_back() {
		if (m_finish != m_end_of_storage) {
			construct(m_finish);
			++m_finish;
		}
		else {
			insert_aux(end());
		}
	}
	void push_front(const Tp& x) = delete;
	inline void pop_back() {
		--m_finish;
		destroy(m_finish);
	}
	void pop_front() = delete;

	void swap(vector<Tp, Alloc>& x) {
		std::swap(m_start, x.m_start);
		std::swap(m_finish, x.m_finish);
		std::swap(m_end_of_storage, x.m_end_of_storage);
	}
	
	iterator insert(iterator pos, const Tp& value) {
		size_type n = pos - begin();
		if (m_finish != m_end_of_storage && pos == m_finish) {
			construct(m_finish, value);
			++m_finish;
		}
		else {
			insert_aux(pos, value);
		}
		return begin() + n;
	}
	iterator insert(iterator pos) {
		size_type n = pos - begin();
		if (m_finish != m_end_of_storage && pos == m_finish) {
			construct(m_finish);
			++m_finish;
		}
		else {
			insert_aux(pos);
		}
		return begin() + n;
	}

	void insert(iterator pos, const_iterator first, const_iterator last) {
		if (first == last) return;
		size_type n = 0;
		distance(first, last, n);
		if (size_type(m_end_of_storage - m_finish) >= n) {
			const size_type ele_after = size_type(m_finish - pos);
			iterator old = m_finish;
			if (ele_after > n) {
				uninitialized_copy(m_finish - n, m_finish, m_finish);
				m_finish += n;
				copy_backward(pos, old - n, old);
				copy(first, last, pos);
			}
			else {
				uninitialized_copy(first + ele_after, last, m_finish);
				m_finish += (n - ele_after);
				uninitialized_copy(pos, old, m_finish);
				m_finish += ele_after;
				copy(first, first + ele_after, pos);
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + max(old_size, n);
			iterator new_start = m_allocator(len);
			iterator new_finish = new_start;
			// copy and insert
			new_finish = uninitialized_copy(m_start, pos, new_start);
			new_finish = uninitialized_copy(first, last, new_finish);
			new_finish = uninitialized_copy(pos, m_finish, new_finish);
			// free previous block
			destroy(m_start, m_finish);
			m_deallocator(m_start, m_end_of_storage);
			m_start = new_start;
			m_finish = new_finish;
			m_end_of_storage = m_start + len;
		}
	}

	inline void insert(iterator pos, size_type n, const Tp& x) {
		fill_insert(pos, n, x);}

	iterator erase(iterator pos) {
		if (pos + 1 != end()) {
			copy(pos + 1, m_finish, pos);
		}
		--m_finish;
		destroy(m_finish);
		return pos;
	}
	iterator erase(iterator first, iterator last) {
		iterator it = copy(last, m_finish, first);
		destroy(it, m_finish);
		m_finish = m_finish - (last - first);
		return first;
	}

	void resize(size_type new_size, const Tp& x = Tp()) {
		if (new_size < size()) {
			erase(begin() + new_size, end());
		}
		else {
			insert(end(), new_size - size(), x);
		}
	}

	// override
	inline reference operator[](size_type n) { return *(begin() + n); }
	inline const_reference operator[](size_type n)const { return *(begin() + n); }
	vector& operator=(const vector<Tp, Alloc>&x) {
		if (&x == this) return*this;
		const size_type len = x.size();
		if (len > capacity()) {
			// len > capacity
			iterator tmp = allocate_and_copy(len, x.begin(), x.end());
			destroy(m_start, m_finish);
			m_deallocator(m_start, m_end_of_storage - m_start);
			m_start = tmp;
			m_end_of_storage = m_start + len;
		}
		else if (size() >= len) {
			// len < size()
			iterator it = copy(x.begin(), x.end(), begin());
			destroy(it, m_finish);
		}
		else {
			// len > size() && len < capacity
			copy(x.begin(), x.begin() + size(), m_start);
			uninitialized_copy(x.begin() + size(), x.end(), m_finish);
		}
		m_finish = m_start + len;
		return*this;
	}
	void reserve(const size_type n) {
		if (capacity() >= n)return;
		const size_type old = size();
		iterator tmp = allocate_and_copy(n, m_start, m_finish);
		destroy(m_start, m_finish);
		m_deallocator(m_start, (size_t)(m_end_of_storage - m_start));
		m_start = tmp;
		m_finish = tmp + old;
		m_end_of_storage = tmp + n;
	}
	inline void clear() { erase(begin(), end()); }

protected:
	iterator allocate_and_copy(size_type n, const_iterator first, const_iterator last) {
		iterator result = m_allocator(n);
		uninitialized_copy(first, last, result);
		return result;
	}
	void fill_assign(size_type n, const Tp& value) {
		if (n > capacity()) {
			vector<Tp, Alloc>tmp(n, value);
			tmp.swap(*this);
		}
		else if (n > size()) {
			fill(begin(), end(), value);
			m_finish = uninitialized_fill_n(m_finish, n - size(), value);
		}
		else {
			// n < size()
			erase(fill_n(begin(), n, value), end());
		}
	}
	void insert_aux(iterator pos, const Tp& value) {
		if (m_finish != m_end_of_storage) {
			construct(m_finish, *(m_finish - 1));
			++m_finish;
			Tp val_copy = value;
			copy_backward(pos, m_finish - 2, m_finish - 1);
			*pos = val_copy;
		}
		else {
			const size_type old = size();
			const size_type len = old != 0 ? (old << 1) : 1;
			iterator start = m_allocator(len);
			iterator finish = uninitialized_copy(m_start, pos, start);
			// insert
			construct(finish, value);
			++finish;
			finish = uninitialized_copy(pos, m_finish, finish);
			// free
			destroy(begin(), end());
			m_deallocator(m_start, m_end_of_storage - m_start);
			m_start = start;
			m_finish = finish;
			m_end_of_storage = m_start + len;
		}
	}
	void insert_aux(iterator pos) {
		if (m_finish != m_end_of_storage) {
			construct(m_finish, *(m_finish - 1));
			++m_finish;
			copy_backward(pos, m_finish - 2, m_finish - 1);
			*pos = Tp();
		}
		else {
			const size_type old = size();
			const size_type len = old != 0 ? (old << 1) : 1;
			iterator start = m_allocator(len);
			iterator finish = uninitialized_copy(m_start, pos, start);
			// insert
			construct(finish);
			++finish;
			finish = uninitialized_copy(pos, m_finish, finish);
			// free
			destroy(begin(), end());
			m_deallocator(m_start, m_end_of_storage - m_start);
			m_start = start;
			m_finish = finish;
			m_end_of_storage = m_start + len;
		}
	}
	void fill_insert(iterator pos, size_type n, const Tp& x) {
		if (n == 0) return;
		if (size_type(m_end_of_storage - m_finish) > n) {
			const size_type ele_after = size_type(m_finish - pos);
			iterator old = m_finish;
			if (ele_after > n) {
				uninitialized_copy(m_finish - n, m_finish, m_finish);
				m_finish += n;
				copy_backward(pos, old - n, old);
				fill(pos, pos + n, x);
			}
			else {
				uninitialized_fill_n(m_finish, n - ele_after, x);
				m_finish += (n - ele_after);
				uninitialized_copy(pos, old, m_finish);
				m_finish += ele_after;
				fill(pos, old, x);
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + max(old_size, n);
			iterator new_start = m_allocator(len);
			iterator new_finish = new_start;
			// start copy and fill
			new_finish = uninitialized_copy(m_start, pos, new_start);
			new_finish = uninitialized_fill_n(new_finish, n, x);
			new_finish = uninitialized_copy(pos, m_finish, new_finish);
			// free the previous block
			destroy(m_start, m_finish);
			m_deallocator(m_start, (size_type)(m_end_of_storage - m_start));
			m_start = new_start;
			m_finish = new_finish;
			m_end_of_storage = new_start + len;
		}
	}

};

template<typename Tp, typename Alloc>
inline bool
operator==(const vector<Tp, Alloc>& x, const vector<Tp, Alloc>& y) {
	return x.size() == y.size() &&
		   equal(x.begin(), x.end(), y.begin(), y.end());
}

template<typename Tp, typename Alloc>
inline bool
operator<(const vector<Tp, Alloc>& x, const vector<Tp, Alloc>& y) {
	return lexigraphical_compare(x.begin(), x.end(), 
		                         y.begin(), y.end());
}

NAMESPACE_END

#endif
