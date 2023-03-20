#ifndef LIST_TEST_H
#define LIST_TEST_H

#include <cassert>
#include "../src/stl_list.h"

using namespace stl;
class ListTest {
protected:
	static void list1() {
		int array1[] = { 9, 16, 36 };
		int array2[] = { 1, 4 };

		list<int> l1(array1, array1 + 3);
		list<int> l2(array2, array2 + 2);
		list<int>::iterator i1 = l1.begin();
		list<int>::iterator i2 = l2.begin();
		list<int>::const_iterator ci(i1);
		list<int>::const_iterator ci1(ci);
		l1.splice(i1, l2);
		i1 = l1.begin();
		assert(*i1++ == 1);
		assert(*i1++ == 4);
		assert(*i1++ == 9);
		assert(*i1++ == 16);
		assert(*i1++ == 36);
		assert(i2 == l1.begin());
		// default construct check
		list<int> l(2);
		i1 = l.begin();
		assert(*(i1++) == 0);
		assert(*i1 == 0);

	}

	static void list2() {
		int array1[] = { 1, 16 };
		int array2[] = { 4, 9 };

		list<int> l1(array1, array1 + 2);
		list<int> l2(array2, array2 + 2);
		list<int>::iterator i = l1.begin();
		i++;
		l1.splice(i, l2.begin(), l2.end());
		i = l1.begin();
		assert(*i++ == 1);
		assert(*i++ == 4);
		assert(*i++ == 9);
		assert(*i++ == 16);
	}

	static void list3()
	{
		char array[] = { 'x', 'l', 'x', 't', 's', 's' };

		list<char> str(array, array + 6);
		list<char>::iterator i;

		str.reverse();
		i = str.begin();
		assert(*i++ == 's');
		assert(*i++ == 's');
		assert(*i++ == 't');
		assert(*i++ == 'x');
		assert(*i++ == 'l');
		assert(*i++ == 'x');

		str.remove('x');
		i = str.begin();
		assert(*i++ == 's');
		assert(*i++ == 's');
		assert(*i++ == 't');
		assert(*i++ == 'l');

		str.unique();
		i = str.begin();
		assert(*i++ == 's');
		assert(*i++ == 't');
		assert(*i++ == 'l');

		str.sort();
		i = str.begin();
		assert(*i++ == 'l');
		assert(*i++ == 's');
		assert(*i++ == 't');
	}

	static void list4()
	{
		int array1[] = { 1, 3, 6, 7 };
		int array2[] = { 2, 4 };

		list<int> l1(array1, array1 + 4);
		list<int> l2(array2, array2 + 2);
		l1.merge(l2);
		list<int>::iterator i = l1.begin();
		assert(*i++ == 1);
		assert(*i++ == 2);
		assert(*i++ == 3);
		assert(*i++ == 4);
		assert(*i++ == 6);
		assert(*i++ == 7);

		//We use distance to avoid a simple call to an internal counter
		size_t n = 0;
		distance(l1.begin(), l1.end(), n);
		assert(n == 6);
		distance(l2.begin(), l2.end(), n);
		assert(n == 0);

		l1.swap(l2);
		distance(l1.begin(), l1.end(), n);
		assert(n == 0);
		distance(l2.begin(), l2.end(), n);
		assert(n == 6);
	}

	static void erase() {
		list<int> l;
		l.push_back(1);
		l.erase(l.begin());
		assert(l.empty());

		int array[] = { 0, 1, 2, 3 };
		l.insert(l.begin(), array, array + 4);
		list<int>::iterator lit;
		lit = l.erase(l.begin());
		assert(*lit == 1);

		lit = l.erase(l.begin(), --l.end());
		assert(*lit == 3);

		l.clear();
		assert(l.empty());
	}

	static void resize() {
		{
			list<int> l;
			l.resize(5, 1);

			size_t i;
			list<int>::iterator lit(l.begin());
			for (i = 0; i < 5; ++i) {
				assert(lit != l.end());
				assert(*(lit++) == 1);
			}
			assert(lit == l.end());

			l.resize(3);
			lit = l.begin();
			for (i = 0; i < 3; ++i) {
				assert(lit != l.end());
				assert(*(lit++) == 1);
			}
			assert(lit == l.end());
		}
		{
			list<int> l;
			l.resize(5);

			size_t i;
			list<int>::iterator lit(l.begin());
			for (i = 0; i < 5; ++i) {
				assert(lit != l.end());
				assert(*(lit++) == 0);
			}
			assert(lit == l.end());
		}
	}

	static void push_back() {
		list<int> l;
		l.push_back(1);
		l.push_back(2);
		l.push_back(3);

		list<int>::reverse_iterator r = l.rbegin();

		assert(*r == 3);
		l.push_back(4);
	}

	static void push_front()
	{
		list<int> l;
		l.push_back(1);
		l.push_back(2);
		l.push_back(3);

		list<int>::iterator i = l.begin();

		assert(*i == 1);
		l.push_front(0);
		assert(*i == 1);
		++i;
		assert(*i == 2);
	}

	static void swap()
	{
		list<int> lst1;
		list<int> lst2;

		lst1.push_back(1);
		lst2.push_back(2);

		lst1.swap(lst2);

		assert(lst1.front() == 2);
		assert(lst2.front() == 1);
		assert(lst1.size() == 1);
		assert(lst2.size() == 1);

		lst1.pop_front();
		lst2.pop_front();

		assert(lst1.empty());
		assert(lst2.empty());
	}

public:
	static void run() {
		list1();
		list2();
		list3();
		list4();
		erase();
		resize();
		push_back();
		push_front();
		swap();
	}
};

#endif // LIST_TEST_H