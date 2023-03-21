#ifndef MAP_TEST_H
#define MAP_TEST_H

#include "../src/stl_function.h"
#include "../src/multimap.h"
#include "../src/stl_map.h"

using namespace stl;

class MapTest {
protected:
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
protected:
	static void map1() {
		typedef map<char, int, less<char>> maptype;
		maptype m;
		// Store mappings between roman numerals and decimals.
		m['l'] = 50;
		m['x'] = 20; // Deliberate mistake.
		m['v'] = 5;
		m['i'] = 1;
		//  cout << "m['x'] = " << m['x'] << endl;
		assert(m['x'] == 20);
		m['x'] = 10; // Correct mistake.
		assert(m['x'] == 10);
		assert(m['z'] == 0);
		//cout << "m['z'] = " << m['z'] << endl; // Note default value is added.
		assert(m.count('z') == 1);
		//cout << "m.count('z') = " << m.count('z') << endl;
		pair<maptype::iterator, bool> p = m.insert(pair<const char, int>('c', 100));
		assert(p.second);
		assert(p.first != m.end());
		assert((*p.first).first == 'c');
		assert((*p.first).second == 100);

		p = m.insert(pair<const char, int>('c', 100));
		assert(!p.second); // already existing pair
		assert(p.first != m.end());
		assert((*p.first).first == 'c');
		assert((*p.first).second == 100);
	}
	static void mmap1() {
		typedef multimap<char, int, less<char> > mmap;
		mmap m;
		assert(m.count('X') == 0);

		m.insert(pair<const char, int>('X', 10)); // Standard way.
		assert(m.count('X') == 1);

		m.insert(pair<const char, int>('X', 20)); // jbuck: standard way
		assert(m.count('X') == 2);

		m.insert(pair<const char, int>('Y', 32)); // jbuck: standard way
		mmap::iterator i = m.find('X'); // Find first match.
		pair<const char, int> p('X', 10);
		assert(*i == p);
		assert((*i).first == 'X');
		assert((*i).second == 10);
		i++;
		assert((*i).first == 'X');
		assert((*i).second == 20);
		i++;
		assert((*i).first == 'Y');
		assert((*i).second == 32);
		i++;
		assert(i == m.end());

		size_t count = m.erase('X');
		assert(count == 2);
	}
	static void mmap2() {
		typedef pair<const int, char> pair_type;

		pair_type p1(3, 'c');
		pair_type p2(6, 'f');
		pair_type p3(1, 'a');
		pair_type p4(2, 'b');
		pair_type p5(3, 'x');
		pair_type p6(6, 'f');

		typedef multimap<int, char, less<int> > mmap;

		pair_type array[] = {
		  p1,
		  p2,
		  p3,
		  p4,
		  p5,
		  p6
		};

		mmap m(array + 0, array + 6);
		mmap::iterator i;
		i = m.lower_bound(3);
		assert((*i).first == 3);
		assert((*i).second == 'c');

		i = m.upper_bound(3);
		assert((*i).first == 6);
		assert((*i).second == 'f');
	}

	static void iterators() {
		typedef map<int, char, less<int>> int_map;
		int_map imap;
		{
			int_map::iterator ite(imap.begin());
			int_map::const_iterator cite(imap.begin());
			assert(ite == cite);
			assert(!(ite != cite));
			assert(cite == ite);
			assert(!(cite != ite));
		}

		typedef multimap<int, char, less<int> > mmap;
		typedef mmap::value_type pair_type;

		pair_type p1(3, 'c');
		pair_type p2(6, 'f');
		pair_type p3(1, 'a');
		pair_type p4(2, 'b');
		pair_type p5(3, 'x');
		pair_type p6(6, 'f');

		pair_type array[] = {
		  p1,
		  p2,
		  p3,
		  p4,
		  p5,
		  p6
		};

		mmap m(array + 0, array + 6);

		{
			mmap::iterator ite(m.begin());
			mmap::const_iterator cite(m.begin());
			//test compare between const_iterator and iterator
			assert(ite == cite);
			assert(!(ite != cite));
			assert(cite == ite);
			assert(!(cite != ite));
		}
		mmap::reverse_iterator ri = m.rbegin();
		assert(ri != m.rend());
		assert(ri == m.rbegin());
		assert((*ri).first == 6);
		assert((*ri++).second == 'f');
		assert((*ri).first == 6);
		assert((*ri).second == 'f');

		mmap const& cm = m;
		mmap::const_reverse_iterator rci = cm.rbegin();
		assert(rci != cm.rend());
		assert((*rci).first == 6);
		assert((*rci++).second == 'f');
		assert((*rci).first == 6);
		assert((*rci).second == 'f');
	}
	
	static void equal_range() {
		typedef map<char, int, less<char> > maptype;
		{
			maptype m;
			m['x'] = 10;

			pair<maptype::iterator, maptype::iterator> ret;
			ret = m.equal_range('x');
			assert(ret.first != ret.second);
			assert((*(ret.first)).first == 'x');
			assert((*(ret.first)).second == 10);
			assert(++(ret.first) == ret.second);
		}
		{
			{
				maptype m;

				maptype::iterator i = m.lower_bound('x');
				assert(i == m.end());

				i = m.upper_bound('x');
				assert(i == m.end());

				pair<maptype::iterator, maptype::iterator> ret;
				ret = m.equal_range('x');
				assert(ret.first == ret.second);
				assert(ret.first == m.end());
			}

			{
				const maptype m;
				pair<maptype::const_iterator, maptype::const_iterator> ret;
				ret = m.equal_range('x');
				assert(ret.first == ret.second);
				assert(ret.first == m.end());
			}
		}
	}
	
	static void template_methods() {
		{
			typedef map<Key, int, KeyCmp> Container;
			typedef Container::value_type value;
			Container cont;
			cont.insert(value(Key(1), 1));
			cont.insert(value(Key(2), 2));
			cont.insert(value(Key(3), 3));
			cont.insert(value(Key(4), 4));

			assert(cont.count(Key(1)) == 1);
			assert(cont.count(Key(1)) == 1);
			assert(cont.count(Key(5)) == 0);

			assert(cont.find(Key(2)) != cont.end());
			assert(cont.lower_bound(Key(2)) != cont.end());
			assert(cont.upper_bound(Key(2)) != cont.end());
			assert(cont.equal_range(Key(2)) != make_pair(cont.begin(), cont.end()));

			Container const& ccont = cont;
			assert(ccont.find(Key(2)) != ccont.end());
			assert(ccont.lower_bound(Key(2)) != ccont.end());
			assert(ccont.upper_bound(Key(2)) != ccont.end());
			assert(ccont.equal_range(Key(2)) != make_pair(ccont.end(), ccont.end()));
		}
		{
			typedef multimap<Key, int, KeyCmp> Container;
			typedef Container::value_type value;
			Container cont;
			cont.insert(value(Key(1), 1));
			cont.insert(value(Key(2), 2));
			cont.insert(value(Key(3), 3));
			cont.insert(value(Key(4), 4));

			assert(cont.count(Key(1)) == 1);
			assert(cont.count(Key(1)) == 1);
			assert(cont.count(Key(5)) == 0);

			assert(cont.find(Key(2)) != cont.end());
			assert(cont.lower_bound(Key(2)) != cont.end());
			assert(cont.upper_bound(Key(2)) != cont.end());
			assert(cont.equal_range(Key(2)) != make_pair(cont.begin(), cont.end()));

			Container const& ccont = cont;
			assert(ccont.find(Key(2)) != ccont.end());
			assert(ccont.lower_bound(Key(2)) != ccont.end());
			assert(ccont.upper_bound(Key(2)) != ccont.end());
			assert(ccont.equal_range(Key(2)) != make_pair(ccont.end(), ccont.end()));
		}

	}
public:
	static void run() {
		map1();
		mmap1();
		mmap2();
		iterators();
		equal_range();
		template_methods();
	}

};

#endif // MAP_TEST_H