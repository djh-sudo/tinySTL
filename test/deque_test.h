#ifndef DEQUE_TEST_H
#define DEQUE_TEST_H

#include <cassert>
#include "../src/stl_deque.h"

using namespace stl;

class DequeTest {
protected:
	static void deque1() {
		deque<int> d;
		d.push_back(4);
		d.push_back(9);
		d.push_back(16);
		d.push_front(1);

		assert(d[0] == 1);
		assert(d[1] == 4);
		assert(d[2] == 9);
		assert(d[3] == 16);

		d.pop_front();
		d[2] = 25;

		assert(d[0] == 4);
		assert(d[1] == 9);
		assert(d[2] == 25);
	}
	static void insert() {
		deque<int> d;
		d.push_back(0);
		d.push_back(1);
		d.push_back(2);
		assert(d.size() == 3);

		deque<int>::iterator dit;
		//Insertion before begin:
		dit = d.insert(d.begin(), 3);
		assert(dit != d.end());
		assert(*dit == 3);
		assert(d.size() == 4);
		assert(d[0] == 3);

		//Insertion after begin:
		dit = d.insert(d.begin() + 1, 4);
		assert(dit != d.end());
		assert(*dit == 4);
		assert(d.size() == 5);
		assert(d[1] == 4);

		//Insertion at end:
		dit = d.insert(d.end(), 5);
		assert(dit != d.end());
		assert(*dit == 5);
		assert(d.size() == 6);
		assert(d[5] == 5);

		//Insertion before last element:
		dit = d.insert(d.end() - 1, 6);
		assert(dit != d.end());
		assert(*dit == 6);
		assert(d.size() == 7);
		assert(d[5] == 6);

		//Insertion of several elements before begin
		d.insert(d.begin(), 2, 7);
		assert(d.size() == 9);
		assert(d[0] == 7);
		assert(d[1] == 7);

		//Insertion of several elements after begin
		//There is more elements to insert than elements before insertion position
		d.insert(d.begin() + 1, 2, 8);
		assert(d.size() == 11);
		assert(d[1] == 8);
		assert(d[2] == 8);

		//There is less elements to insert than elements before insertion position
		d.insert(d.begin() + 3, 2, 9);
		assert(d.size() == 13);
		assert(d[3] == 9);
		assert(d[4] == 9);

		//Insertion of several elements at end:
		d.insert(d.end(), 2, 10);
		assert(d.size() == 15);
		assert(d[14] == 10);
		assert(d[13] == 10);

		//Insertion of several elements before last:
		//There is more elements to insert than elements after insertion position
		d.insert(d.end() - 1, 2, 11);
		assert(d.size() == 17);
		assert(d[15] == 11);
		assert(d[14] == 11);

		//There is less elements to insert than elements after insertion position
		d.insert(d.end() - 3, 2, 12);
		assert(d.size() == 19);
		assert(d[15] == 12);
		assert(d[14] == 12);
	}

	static void auto_ref() {
		int i;
		deque<int> ref;
		for (i = 0; i < 5; ++i) {
			ref.push_back(i);
		}
		deque<deque<int>> d_d_int(1, ref);
		d_d_int.push_back(d_d_int[0]);
		d_d_int.push_back(ref);
		d_d_int.push_back(d_d_int[0]);
		d_d_int.push_back(d_d_int[0]);
		d_d_int.push_back(ref);

		for (i = 0; i < 5; ++i) {
			assert(d_d_int[i] == ref);
		}
	}

	static void erase() {
		deque<int> dint;
		dint.push_back(3);
		dint.push_front(2);
		dint.push_back(4);
		dint.push_front(1);
		dint.push_back(5);
		dint.push_front(0);
		dint.push_back(6);

		deque<int>::iterator it(dint.begin() + 1);
		assert(*it == 1);

		dint.erase(dint.begin());
		assert(*it == 1);

		it = dint.end() - 2;
		assert(*it == 5);

		dint.erase(dint.end() - 1);
		assert(*it == 5);

		dint.push_back(6);
		dint.push_front(0);

		it = dint.begin() + 2;
		assert(*it == 2);

		dint.erase(dint.begin(), dint.begin() + 2);
		assert(*it == 2);

		it = dint.end() - 3;
		assert(*it == 4);

		dint.erase(dint.end() - 2, dint.end());
		assert(*it == 4);
	}

public:
	static void run() {
		deque1();
		insert();
		auto_ref();
		erase();
	}
};
#endif // DEQUE_TEST_H