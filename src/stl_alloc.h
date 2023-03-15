#ifndef STL_ALLOC_H
#define STL_ALLOC_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "stl_config.h"

#define _OOM_ "out of memory!!!"


NAMESPACE_START
class MallocAlloc {

public:
	inline static void* allocate(size_t n) {
		void* result = ::malloc(n);
		if (nullptr == result) throw _OOM_;
		return result;
	}

	inline static void deallocate(void* p, size_t /*pad*/) {
		::free(p);
	}

	inline static void* reallocate(void* p, size_t/*old_Sz*/, size_t new_sz) {
		void* result = ::realloc(p, new_sz);
		if (nullptr == result) throw _OOM_;
		return result;
	}
};


class DefaultAlloc {

public:
	static void* allocate(size_t n) {
		void* ret = nullptr;
		if (n <= 0) {
			return nullptr;
		}
		else if (n > (size_t)MAX_BYTES) {
			// primary allocator
			ret = MallocAlloc::allocate(n);
		}
		else {
			Obj** my_list = m_free_list + free_list_index(n);
			Obj* result = *my_list;
			if (nullptr == result) {
				ret = refill(round_up(n));
			}
			else {
				*my_list = result->m_free_list_link;
				ret = result;
			}
		}
		return ret;
	}

	static void deallocate(void* p, size_t n) {
		if (p == nullptr || n <= 0) return;
		else if (n > (size_t)MAX_BYTES) {
			// primary deallocator
			MallocAlloc::deallocate(p, n);
		}
		else {
			Obj** my_list = m_free_list + free_list_index(n);
			Obj* q = (Obj*)p;
			q->m_free_list_link = *my_list;
			*my_list = q;
		}
	}

	static void* reallocate(void*p, size_t old_sz, size_t new_sz) {
		if (old_sz > (size_t)MAX_BYTES && new_sz > (size_t)MAX_BYTES) {
			return ::realloc(p, new_sz);
		}
		else if (round_up(old_sz) == round_up(new_sz)) {
			return p;
		}

		void* result = allocate(new_sz);
		size_t copy_sz = new_sz > old_sz ? old_sz : new_sz;
		::memcpy(result, p, copy_sz);
		deallocate(p, old_sz);
		return result;
	}

private:
	static size_t round_up(size_t bytes) {
		return (bytes + (size_t)ALIGN - 1) & (~((size_t)ALIGN - 1));}

	static size_t free_list_index(size_t bytes) {
		return (bytes + (size_t)ALIGN - 1) / ((size_t)ALIGN) - 1;}

	static void* refill(size_t n) {
		int njobs = 20;
		char* chunck = chunk_alloc(n, njobs);
		if (1 == njobs) return chunck;
		Obj** my_list = m_free_list + free_list_index(n);
		Obj* result = (Obj*)chunck;
		Obj* next_obj = (Obj*)(chunck + n);
		*my_list = next_obj;
		for (int i = 1;; ++i) {
			Obj* cur_job = next_obj;
			next_obj = (Obj*)((char*)next_obj + n);
			if (njobs - 1 == i) {
				cur_job->m_free_list_link = nullptr;
				break;
			}
			else {
				cur_job->m_free_list_link = next_obj;
			}
		}
		return result;
	}

	static char* chunk_alloc(size_t size, int& njobs) {
		size_t total_bytes = size * njobs;
		size_t left_bytes = size_t(m_end_free - m_start_free);
		if (left_bytes >= total_bytes) {
			// the pool capacity is enough!
			char* result = m_start_free;
			m_start_free += total_bytes;
			return result;
		}
		else if (left_bytes >= size) {
			njobs = (int)(left_bytes / size);
			total_bytes = size * njobs;
			char* result = m_start_free;
			m_start_free += total_bytes;
			return result;
		}
		else {
			// the pool capacity is not enough!
			if (left_bytes > 0) {
				// memory segment, merge it!
				Obj** my_list = m_free_list + free_list_index(left_bytes);
				((Obj*)m_start_free)->m_free_list_link = *my_list;
				*my_list = (Obj*)m_start_free;
			}
			// allocate more memory, add them into pool
			size_t get_bytes = 2 * total_bytes + round_up(m_heap_size >> 4);
			m_start_free = (char*)::malloc(get_bytes);
			if (nullptr == m_start_free) {
				// allocate failed
				Obj** my_list = nullptr;
				Obj* p = nullptr;
				for (size_t i = size; i <= (size_t)MAX_BYTES; i += (size_t)ALIGN) {
					// find bigger block
					my_list = m_free_list + free_list_index(i);
					p = *my_list;
					if (nullptr != p) {
						*my_list = p->m_free_list_link;
						m_start_free = (char*)p;
						m_end_free = m_start_free + i;
						return chunk_alloc(size, njobs);
					}
				}
				// can not find free block
				m_end_free = nullptr;
				// ask primary allocator!
				m_start_free = (char*)MallocAlloc::allocate(get_bytes);
				// it must succeed, or will throw OOM
			}
			m_heap_size += get_bytes;
			m_end_free = m_start_free + get_bytes;
			return chunk_alloc(size, njobs);
		}
	}
public:
	inline static size_t getHeapSize() {
		return m_heap_size;
	}

private:
	enum {
		ALIGN = 8, MAX_BYTES = 128, NFREELISTS = 16 };
	
	struct Obj {
		struct Obj* m_free_list_link;
	};

	static Obj* m_free_list[NFREELISTS];

	static char* m_start_free;

	static char* m_end_free;

	static size_t m_heap_size;
};

// initialize
char* DefaultAlloc::m_start_free = nullptr;

char* DefaultAlloc::m_end_free = nullptr;

size_t DefaultAlloc::m_heap_size = 0;

DefaultAlloc::Obj*
DefaultAlloc::m_free_list[NFREELISTS] =
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

typedef DefaultAlloc alloc;

template<typename Tp, typename Alloc>
class SimpleAlloc {

public:
	static Tp* allocate(size_t n) {
		return n == 0 ? nullptr : (Tp*)Alloc::allocate(n * sizeof(Tp)); }
	static Tp* allocate(void) { 
		return (Tp*)Alloc::allocate(sizeof(Tp)); }
	static void deallocate(Tp* p, size_t n) {
		if (n != 0 && p != nullptr)Alloc::deallocate(p, n * sizeof(Tp)); }
	static void deallocate(Tp* p) {
		if (p != nullptr) Alloc::deallocate(p, sizeof(Tp)); }
};

NAMESPACE_END

#endif // ifndef STL_ALLOC_H
