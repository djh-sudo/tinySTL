#ifndef SET_TEST_H
#define SET_TEST_H
#include "../src/stl_set.h"
#include "../src/stl_multiset.h"

using namespace stl;

class SetTestClass {
public:
	SetTestClass(int data) : _data(data) {}
	int data() const { return _data; }
private:
	int _data;
};

namespace stl {
	template <>
	struct less<SetTestClass> {
		bool operator () (SetTestClass const& lhs, SetTestClass const& rhs) const {
			return lhs.data() < rhs.data();
		}
	};
};

struct Key {
	Key() : m_data(0) {}
	explicit Key(int data) : m_data(data) {}
	int m_data;
};

struct KeyCmp {
	bool operator () (Key lhs, Key rhs) const {
		return lhs.m_data < rhs.m_data;
	}

	bool operator () (Key lhs, int rhs) const {
		return lhs.m_data < rhs;
	}

	bool operator () (int lhs, Key rhs) const {
		return lhs < rhs.m_data;
	}
};

struct KeyCmpPtr {
	bool operator () (Key const volatile* lhs, Key const volatile* rhs) const {
		return (*lhs).m_data < (*rhs).m_data;
	}

	bool operator () (Key const volatile* lhs, int rhs) const {
		return (*lhs).m_data < rhs;
	}

	bool operator () (int lhs, Key const volatile* rhs) const {
		return lhs < (*rhs).m_data;
	}
};

class SetTest {
protected:
	static void set1() {
		set<int, less<int> > s;
		assert(s.count(42) == 0);
		s.insert(42);
		assert(s.count(42) == 1);
		s.insert(42);
		assert(s.count(42) == 1);
		size_t count = s.erase(42);
		assert(count == 1);
	}

	static void set2() {
		typedef set<int, less<int> > int_set;
		int_set s;
		pair<int_set::iterator, bool> p = s.insert(42);
		assert(p.second == true);
		p = s.insert(42);
		assert(p.second == false);

		int array1[] = { 1, 3, 6, 7 };
		s.insert(array1, array1 + 4);
		size_t n = 0;
		distance(s.begin(), s.end(), n);
		assert(n == 5);

		int_set s2;
		s2.swap(s);
		distance(s2.begin(), s2.end(), n);
		assert(n == 5);
		distance(s.begin(), s.end(), n);
		assert(n == 0);

		int_set s3;
		s3.swap(s);
		s3.swap(s2);
		distance(s.begin(), s.end(), n);
		assert(n == 0);
		distance(s2.begin(), s2.end(), n);
		assert(n == 0);
		distance(s3.begin(), s3.end(), n);
		assert(n == 5);
	}

	static void erase() {
		set<int, less<int> > s;
		s.insert(1);
		s.erase(s.begin());
		assert(s.empty());
		size_t nb = s.erase(1);
		assert(nb == 0);
	}

	static void insert() {
		set<int> s;
		set<int>::iterator i = s.insert(s.end(), 0);
		assert(*i == 0);
	}

	static void find() {
		set<int> s;
		assert(s.find(0) == s.end());
		set<int> const& crs = s;
		assert(crs.find(0) == crs.end());
	}

	static void bounds() {
		int array1[] = { 1, 3, 6, 7 };
		set<int> s(array1, array1 + sizeof(array1) / sizeof(array1[0]));
		set<int> const& crs = s;

		set<int>::iterator sit;
		set<int>::const_iterator scit;
		pair<set<int>::iterator, set<int>::iterator> pit;
		pair<set<int>::const_iterator, set<int>::const_iterator> pcit;

		//Check iterator on mutable set
		sit = s.lower_bound(2);
		assert(sit != s.end());
		assert(*sit == 3);

		sit = s.upper_bound(5);
		assert(sit != s.end());
		assert(*sit == 6);

		pit = s.equal_range(6);
		assert(pit.first != pit.second);
		assert(pit.first != s.end());
		assert(*pit.first == 6);
		assert(pit.second != s.end());
		assert(*pit.second == 7);

		pit = s.equal_range(4);
		assert(pit.first == pit.second);
		assert(pit.first != s.end());
		assert(*pit.first == 6);
		assert(pit.second != s.end());
		assert(*pit.second == 6);

		//Check const_iterator on mutable set
		scit = s.lower_bound(2);
		assert(scit != s.end());
		assert(*scit == 3);

		scit = s.upper_bound(5);
		assert(scit != s.end());
		assert(*scit == 6);

		//Check const_iterator on const set
		scit = crs.lower_bound(2);
		assert(scit != crs.end());
		assert(*scit == 3);

		scit = crs.upper_bound(5);
		assert(scit != crs.end());
		assert(*scit == 6);

		pcit = crs.equal_range(6);
		assert(pcit.first != pcit.second);
		assert(pcit.first != crs.end());
		assert(*pcit.first == 6);
		assert(pcit.second != crs.end());
		assert(*pcit.second == 7);
	}

	static void specialized_less() {
		set<SetTestClass> s;
		s.insert(SetTestClass(1));
		s.insert(SetTestClass(3));
		s.insert(SetTestClass(2));
		s.insert(SetTestClass(0));

		set<SetTestClass>::iterator sit(s.begin()), sitEnd(s.end());
		int i = 0;
		for (; sit != sitEnd; ++sit, ++i) {
			assert(sit->data() == i);
		}
	}

	static void implementation_check() {
		set<int> tree;
		tree.insert(1);
		set<int>::iterator it = tree.begin();
		int const& int_ref = *it++;
		assert(int_ref == 1);

		assert(it == tree.end());
		assert(it != tree.begin());

		set<int>::const_iterator cit = tree.begin();
		int const& int_cref = *cit++;
		assert(int_cref == 1);
	}

	static void reverse_iterator_test() {
		set<int> tree;
		tree.insert(1);
		tree.insert(2);
		{
			set<int>::reverse_iterator rit(tree.rbegin());
			assert(*(rit++) == 2);
			assert(*(rit++) == 1);
			assert(rit == tree.rend());
		}
		{
			set<int> const& ctree = tree;
			set<int>::const_reverse_iterator rit(ctree.rbegin());
			assert(*(rit++) == 2);
			assert(*(rit++) == 1);
			assert(rit == ctree.rend());
		}
	}

	static void template_methods() {
		typedef set<Key, KeyCmp> KeySet;
		KeySet keySet;
		keySet.insert(Key(1));
		keySet.insert(Key(2));
		keySet.insert(Key(3));
		keySet.insert(Key(4));

		assert(keySet.count(Key(1)) == 1);
		assert(keySet.count(Key(1)) == 1);
		assert(keySet.count(Key(5)) == 0);

		assert(keySet.find(Key(2)) != keySet.end());
		assert(keySet.lower_bound(Key(2)) != keySet.end());
		assert(keySet.upper_bound(Key(2)) != keySet.end());
		assert(keySet.equal_range(Key(2)) != make_pair(keySet.begin(), keySet.end()));

		KeySet const& ckeySet = keySet;
		assert(ckeySet.find(Key(2)) != ckeySet.end());
		assert(ckeySet.lower_bound(Key(2)) != ckeySet.end());
		assert(ckeySet.upper_bound(Key(2)) != ckeySet.end());
		assert(ckeySet.equal_range(Key(2)) != make_pair(ckeySet.begin(), ckeySet.end()));
		
		{
			typedef multiset<Key, KeyCmp> KeySet;
			KeySet keySet;
			keySet.insert(Key(1));
			keySet.insert(Key(2));
			keySet.insert(Key(3));
			keySet.insert(Key(4));

			assert(keySet.count(Key(1)) == 1);
			assert(keySet.count(Key(1)) == 1);
			assert(keySet.count(Key(5)) == 0);

			assert(keySet.find(Key(2)) != keySet.end());
			assert(keySet.lower_bound(Key(2)) != keySet.end());
			assert(keySet.upper_bound(Key(2)) != keySet.end());
			assert(keySet.equal_range(Key(2)) != make_pair(keySet.begin(), keySet.end()));

			KeySet const& ckeySet = keySet;
			assert(ckeySet.find(Key(2)) != ckeySet.end());
			assert(ckeySet.lower_bound(Key(2)) != ckeySet.end());
			assert(ckeySet.upper_bound(Key(2)) != ckeySet.end());
			assert(ckeySet.equal_range(Key(2)) != make_pair(ckeySet.begin(), ckeySet.end()));
		}
	}


public:
	static void run() {
		set1();
		set2();
		erase();
		insert();
		find();
		bounds();
		specialized_less();
		implementation_check();
		reverse_iterator_test();
		template_methods();
	}
};
#endif // SET_TEST_H