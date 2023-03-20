#ifndef VECTOR_TEST_H
#define VECTOR_TEST_H

#include <cassert>
#include "../src/stl_vector.h"

using namespace stl;

class VectorTest {
protected:
	static void vec_test_1()
	{
		vector<int> v1; // Empty vector of integers.

		assert(v1.empty() == true);
		assert(v1.size() == 0);

		// assert( v1.max_size() == INT_MAX / sizeof(int) );
		// cout << "max_size = " << v1.max_size() << endl;
		v1.push_back(42); // Add an integer to the vector.
		assert(v1.size() == 1);
		assert(v1[0] == 42);
		{
			vector<vector<int> > vect(10);
			vector<vector<int> >::iterator it(vect.begin()), end(vect.end());
			for (; it != end; ++it) {
				assert((*it).empty());
				assert((*it).size() == 0);
				assert((*it).capacity() == 0);
				assert((*it).begin() == (*it).end());
			}
		}
	}

	static void vec_test_2() {
		vector<double> v1; // Empty vector of doubles.
		v1.push_back(32.1);
		v1.push_back(40.5);
		vector<double> v2; // Another empty vector of doubles.
		v2.push_back(3.56);

		assert(v1.size() == 2);
		assert(v1[0] == 32.1);
		assert(v1[1] == 40.5);

		assert(v2.size() == 1);
		assert(v2[0] == 3.56);
		v1.swap(v2); // Swap the vector's contents.

		assert(v1.size() == 1);
		assert(v1[0] == 3.56);

		assert(v2.size() == 2);
		assert(v2[0] == 32.1);
		assert(v2[1] == 40.5);

		v2 = v1; // Assign one vector to another.

		assert(v2.size() == 1);
		assert(v2[0] == 3.56);
	}

	static void vec_test_3() {
		typedef vector<char> vec_type;
		vec_type v1; // Empty vector of characters.
		v1.push_back('h');
		v1.push_back('i');

		assert(v1.size() == 2);
		assert(v1[0] == 'h');
		assert(v1[1] == 'i');

		vec_type v2(v1.begin(), v1.end());
		v2[1] = 'o'; // Replace second character.

		assert(v2.size() == 2);
		assert(v2[0] == 'h');
		assert(v2[1] == 'o');

		assert((v1 == v2) == false);
		assert((v1 < v2) == true);
	}

	static void vec_test_4() {
		vector<int> v(4);
		v[0] = 1;
		v[1] = 4;
		v[2] = 9;
		v[3] = 16;

		assert(v.front() == 1);
		assert(v.back() == 16);

		v.push_back(25);
		assert(v.back() == 25);
		assert(v.size() == 5);

		v.pop_back();
		assert(v.back() == 16);
		assert(v.size() == 4);
	}

	static void vec_test_5() {
		int array[] = { 1, 4, 9, 16 };
		vector<int> v(array, array + 4);

		assert(v.size() == 4);

		assert(v[0] == 1);
		assert(v[1] == 4);
		assert(v[2] == 9);
		assert(v[3] == 16);
	}

	static void vec_test_6() {
		int array[] = { 1, 4, 9, 16, 25, 36 };
		vector<int> v(array, array + 6);
		vector<int>::iterator vit;

		assert(v.size() == 6);
		assert(v[0] == 1);
		assert(v[1] == 4);
		assert(v[2] == 9);
		assert(v[3] == 16);
		assert(v[4] == 25);
		assert(v[5] == 36);

		vit = v.erase(v.begin()); // Erase first element.
		assert(*vit == 4);

		assert(v.size() == 5);
		assert(v[0] == 4);
		assert(v[1] == 9);
		assert(v[2] == 16);
		assert(v[3] == 25);
		assert(v[4] == 36);

		vit = v.erase(v.end() - 1); // Erase last element.
		assert(vit == v.end());

		assert(v.size() == 4);
		assert(v[0] == 4);
		assert(v[1] == 9);
		assert(v[2] == 16);
		assert(v[3] == 25);
		v.erase(v.begin() + 1, v.end() - 1); // Erase all but first and last.

		assert(v.size() == 2);
		assert(v[0] == 4);
		assert(v[1] == 25);

	}

	static void vec_test_7() {
		int array1[] = { 1, 4, 25 };
		int array2[] = { 9, 16 };

		vector<int> v(array1, array1 + 3);
		vector<int>::iterator vit;
		vit = v.insert(v.begin(), 0); // Insert before first element.
		assert(*vit == 0);

		vit = v.insert(v.end(), 36);  // Insert after last element.
		assert(*vit == 36);

		assert(v.size() == 5);
		assert(v[0] == 0);
		assert(v[1] == 1);
		assert(v[2] == 4);
		assert(v[3] == 25);
		assert(v[4] == 36);

		// Insert contents of array2 before fourth element.
		for (int i = 0; i < 2; ++i) {
			v.insert(v.begin() + 3 + i, *(array2 + i));
		}
		// v.insert(v.begin() + 3, array2, array2 + 2);

		assert(v.size() == 7);
		assert(v[0] == 0);
		assert(v[1] == 1);
		assert(v[2] == 4);
		assert(v[3] == 9);
		assert(v[4] == 16);
		assert(v[5] == 25);
		assert(v[6] == 36);

		v.clear();
		assert(v.empty());
		v.insert(v.begin(), 5, 10);
		assert(v.size() == 5);
		assert(v[0] == 10);
		assert(v[1] == 10);
		assert(v[2] == 10);
		assert(v[3] == 10);
		assert(v[4] == 10);
	}

	struct TestStruct {
		unsigned int a[3];
	};

	static void capacity() {
		vector<int> v;
		assert(v.capacity() == 0);
		v.push_back(42);
		assert(v.capacity() >= 1);
		v.reserve(5000);
		assert(v.capacity() >= 5000);
		vector<TestStruct> va;
		va.reserve(1);
		va.reserve(2);
	}

	static void pointer() {
		vector<int*> v1;
		vector<int*> v2 = v1;
		vector<int*> v3;
		v3.insert(v3.end(), v1.begin(), v1.end());
	}

	static void auto_ref() {
		vector<int> ref;
		for (int i = 0; i < 5; ++i) {
			ref.push_back(i);
		}

		vector<vector<int>> v_v_int(1, ref);
		v_v_int.push_back(v_v_int[0]);
		v_v_int.push_back(ref);
		v_v_int.push_back(v_v_int[0]);
		v_v_int.push_back(v_v_int[0]);
		v_v_int.push_back(ref);

		vector<vector<int> >::iterator vvit(v_v_int.begin()), vvitEnd(v_v_int.end());
		for (; vvit != vvitEnd; ++vvit) {
			assert(*vvit == ref);
		}
	}

	static void iterators() {
		vector<int> vint(10, 0);
		vector<int> const& crvint = vint;
		assert(vint.begin() == vint.begin());
		assert(crvint.begin() == vint.begin());
		assert(vint.begin() == crvint.begin());
		assert(crvint.begin() == crvint.begin());

		assert(vint.begin() != vint.end());
		assert(crvint.begin() != vint.end());
		assert(vint.begin() != crvint.end());
		assert(crvint.begin() != crvint.end());

		assert(vint.rbegin() == vint.rbegin());
		assert(crvint.rbegin() == crvint.rbegin());
		assert(vint.rbegin() != vint.rend());
		assert(crvint.rbegin() != crvint.rend());
	}

public:
	static void run() {
		vec_test_1();
		vec_test_2();
		vec_test_3();
		vec_test_4();
		vec_test_5();
		vec_test_6();
		vec_test_7();
		capacity();
		pointer();
		auto_ref();
		iterators();
	}
};


#endif // VECTOR_TEST_H