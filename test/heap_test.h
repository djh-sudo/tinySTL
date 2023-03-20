#ifndef HEAP_TEST_H
#define HEAP_TEST_H

#include<cassert>
#include "../src/stl_function.h"
#include "../src/stl_heap.h"

using namespace stl;

class HeapTest {
protected:
	static void mkheap0() {
		int numbers[6] = { 5, 10, 4, 13, 11, 19 };
		make_heap(numbers, numbers + 6);
		assert(numbers[0] == 19);
		pop_heap(numbers, numbers + 6);
		assert(numbers[0] == 13);
		pop_heap(numbers, numbers + 5);
		assert(numbers[0] == 11);
		pop_heap(numbers, numbers + 4);
		assert(numbers[0] == 10);
		pop_heap(numbers, numbers + 3);
		assert(numbers[0] == 5);
		pop_heap(numbers, numbers + 2);
		assert(numbers[0] == 4);
		pop_heap(numbers, numbers + 1);
	}
	static void mkheap1() {
		int numbers[6] = { 5, 10, 4, 13, 11, 19 };

		make_heap(numbers, numbers + 6, greater<int>());
		assert(numbers[0] == 4);
		pop_heap(numbers, numbers + 6, greater<int>());
		assert(numbers[0] == 5);
		pop_heap(numbers, numbers + 5, greater<int>());
		assert(numbers[0] == 10);
		pop_heap(numbers, numbers + 4, greater<int>());
		assert(numbers[0] == 11);
		pop_heap(numbers, numbers + 3, greater<int>());
		assert(numbers[0] == 13);
		pop_heap(numbers, numbers + 2, greater<int>());
		assert(numbers[0] == 19);
	}

	static void pheap1() {
		vector<int> v;
		v.push_back(1);
		v.push_back(20);
		v.push_back(4);
		make_heap(v.begin(), v.end());
		v.push_back(7);
		push_heap(v.begin(), v.end());
		sort_heap(v.begin(), v.end());

		assert(v[0] == 1);
		assert(v[1] == 4);
		assert(v[2] == 7);
		assert(v[3] == 20);
	}

	static void pheap2() {
		vector<int> v;
		v.push_back(1);
		v.push_back(20);
		v.push_back(4);
		make_heap(v.begin(), v.end(), greater<int>());
		v.push_back(7);
		push_heap(v.begin(), v.end(), greater<int>());
		sort_heap(v.begin(), v.end(), greater<int>());
		assert(v[0] == 20);
		assert(v[1] == 7);
		assert(v[2] == 4);
		assert(v[3] == 1);
	}

public:
	static void run() {
		mkheap0();
		mkheap1();
		pheap1();
		pheap2();
	}
};

#endif // HEAP_TEST_H