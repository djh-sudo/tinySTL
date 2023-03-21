#ifndef UNORDERED_TEST_H
#define UNORDERED_TEST_H

#include <cassert>
#include <string>
#include "../src/stl_hash_map.h"
#include "../src/stl_hash_set.h"

using namespace stl;
const int NB_ELEMS = 20000;

class UnorderedTest {
protected:
	struct Key {
		Key() : m_data(0) {}
		explicit Key(int data) : m_data(data) {}
		int m_data;
	};

	struct KeyHash{
		size_t operator () (Key key) const {
			return (size_t)key.m_data;
		}
		size_t operator () (int data) const {
			return (size_t)data;
		}
	};

	struct KeyEqual {
		bool operator () (Key lhs, Key rhs) const {
			return lhs.m_data == rhs.m_data;
		}
		bool operator () (Key lhs, int rhs) const {
			return lhs.m_data == rhs;
		}
		bool operator () (int lhs, Key rhs) const {
			return lhs == rhs.m_data;
		}
	};

	struct KeyHashPtr {
		size_t operator () (Key const volatile* key) const {
			return (size_t)key->m_data;
		}
		size_t operator () (int data) const {
			return (size_t)data;
		}
	};

	struct KeyEqualPtr {
		bool operator () (Key const volatile* lhs, Key const volatile* rhs) const {
			return lhs->m_data == rhs->m_data;
		}
		bool operator () (Key const volatile* lhs, int rhs) const {
			return lhs->m_data == rhs;
		}

		bool operator () (int lhs, Key const volatile* rhs) const {
			return lhs == rhs->m_data;
		}
	};

	static void uset() {
		typedef unordered_set<int, hash<int>, equal_to<int>> usettype;
		usettype us;
		//Small compilation check of the copy constructor:
		usettype us2 = us;
		//And assignment operator
		us = us2;

		int i;
		pair<usettype::iterator, bool> ret;
		for (i = 0; i < NB_ELEMS; ++i) {
			ret = us.insert(i);
			assert(ret.second);
			assert(*(ret.first) == i);
			ret = us.insert(i);
			assert(!ret.second);
			assert(*(ret.first) == i);
		}
	}

	static void umultiset() {
		typedef unordered_multiset<int, hash<int>, equal_to<int> > usettype;
		usettype us;
		int i;
		usettype::iterator ret;
		for (i = 0; i < NB_ELEMS; ++i) {
			ret = us.insert(i);
			assert(*ret == i);
			ret = us.insert(i);
			assert(*ret == i);
		}
		assert(us.size() == 2 * NB_ELEMS);
	}

	static void umap() {
		typedef unordered_map<int, int, hash<int>, equal_to<int> > umaptype;
		umaptype us;
		//Compilation check of the [] operator:
		umaptype us2;
		us[0] = us2[0];
		us.clear();
		{
			//An other compilation check
			typedef unordered_map<int, umaptype> uumaptype;
			uumaptype uus;
			umaptype const& uref = uus[0];
			umaptype ucopy = uus[0];
			ucopy = uref;
			//Avoids warning:
			//(void*)&uref;
		}

		int i;
		pair<umaptype::iterator, bool> ret;
		for (i = 0; i < NB_ELEMS; ++i) {
			umaptype::value_type p1(i, i);
			ret = us.insert(p1);
			assert(ret.second);
			assert(*ret.first == p1);

			umaptype::value_type p2(i, i + 1);
			ret = us.insert(p2);
			assert(!ret.second);
			assert(*ret.first == p1);
		}

		{
			//Lets look for some values to see if everything is normal:
			umaptype::iterator umit;
			for (int j = 0; j < NB_ELEMS; j += NB_ELEMS / 100) {
				umit = us.find(j);

				assert(umit != us.end());
				assert((*umit).second == j);
			}
		}
		assert(us.size() == (size_t)NB_ELEMS);
	}

	static void umultimap() {
		typedef unordered_multimap<int, int, hash<int>, equal_to<int> > umaptype;
		umaptype us;
		int i;
		umaptype::iterator ret;
		for (i = 0; i < NB_ELEMS; ++i) {
			umaptype::value_type p(i, i);
			ret = us.insert(p);
			assert(*ret == p);
			ret = us.insert(p);
			assert(*ret == p);
		}
		assert(us.size() == 2 * NB_ELEMS);
		typedef pair<int, int> ptype;
	}

	static void user_case() {
		typedef unordered_map<int, std::string> UnorderedMap1;
		typedef unordered_map<int, UnorderedMap1> UnorderedMap2;

		UnorderedMap1 foo;
		UnorderedMap2 bar;

		foo.insert(UnorderedMap1::value_type(1, std::string("test1")));
		foo.insert(UnorderedMap1::value_type(2, std::string("test2")));
		foo.insert(UnorderedMap1::value_type(3, std::string("test3")));
		foo.insert(UnorderedMap1::value_type(4, std::string("test4")));
		foo.insert(UnorderedMap1::value_type(5, std::string("test5")));

		bar.insert(UnorderedMap2::value_type(0, foo));
		UnorderedMap2::iterator it = bar.find(0);
		assert(it != bar.end());

		UnorderedMap1& body = it->second;
		UnorderedMap1::iterator cur = body.find(3);
		assert(cur != body.end());

		body.erase(body.begin(), body.end());
		assert(body.empty());
	}

	static void buckets() {
		unordered_set<int> int_uset;

		assert(int_uset.bucket_count() < int_uset.max_bucket_count());

		int i;
		size_t nbBuckets = int_uset.bucket_count();
		size_t nbInserts = int_uset.bucket_count() - 1;
		for (i = 0; (size_t)i < nbInserts; ++i) {
			int_uset.insert(i);
		}
		assert(nbBuckets == int_uset.bucket_count());

		size_t bucketSizes = 0;
		for (i = 0; (size_t)i < nbBuckets; ++i) {
			bucketSizes += int_uset.ele_in_bucket(i);
		}
		assert(bucketSizes == int_uset.size());
	}

	static void hash_policy() {
		unordered_set<int> int_uset;

		size_t nbInserts = int_uset.bucket_count() - 1;
		for (int i = 0; (size_t)i < nbInserts; ++i) {
			int_uset.insert(i);
		}
		assert(int_uset.size() == nbInserts);
		size_t bucketsHint = int_uset.bucket_count() + 1;

		assert(int_uset.key_eq()(10, 10));
	}

	static void equal_range() {
		typedef unordered_multiset<size_t> umset;
		{
			//General test
			umset iumset;
			size_t nbBuckets = iumset.bucket_count();

			for (size_t i = 0; i < nbBuckets; ++i) {
				iumset.insert(i);
				iumset.insert(i + nbBuckets);
				iumset.insert(i + 2 * nbBuckets);
				iumset.insert(i + 3 * nbBuckets);
				iumset.insert(i + 4 * nbBuckets);
			}
			// assert(nbBuckets == iumset.bucket_count());
			assert(iumset.size() == 5 * nbBuckets);

			pair<umset::iterator, umset::iterator> p = iumset.equal_range(1);
			assert(p.first != p.second);

			size_t nbElems = iumset.size();
			size_t n = 0;
			distance(p.first, p.second, n);
			nbElems -= n;
			for (umset::iterator j = p.first; j != p.second;) {
				iumset.erase(j++);
			}

			assert(nbElems == iumset.size());

			p = iumset.equal_range(2);
			assert(p.first != p.second);
			distance(p.first, p.second, n);
			nbElems -= n;
			iumset.erase(p.first, p.second);
			assert(nbElems == iumset.size());
		}

		{
			//More specific test that tries to put many values in the same bucket
			umset iumset;

			size_t i;
			const size_t nbBuckets = iumset.bucket_count();
			const size_t targetedBucket = nbBuckets / 2;

			//Lets put 10 values in the targeted bucket:
			for (i = 0; i < 10; ++i) {
				iumset.insert(targetedBucket + (i * nbBuckets));
			}

			//We put again 10 values in the targeted bucket and in reverse order:
			for (i = 9; i <= 10; --i) {
				iumset.insert(targetedBucket + (i * nbBuckets));
			}

			//Now we put some more elements until hash container is resized:
			i = 0;
			while (iumset.bucket_count() == nbBuckets) {
				iumset.insert(i++);
			}

			//assert( iumset.bucket_size(targetedBucket) == 21 );

			pair<umset::iterator, umset::iterator> p = iumset.equal_range(targetedBucket);
			assert(p.first != p.second);
			size_t n = 0;
			distance(p.first, p.second, n);
			assert(n == 3);
		}

		{
			srand(0);
			for (int runs = 0; runs < 2; ++runs) {
				size_t magic = rand();
				umset hum;
				size_t c = 0;
				for (int i = 0; i < 10000; ++i) {
					if ((rand() % 500) == 0) {
						hum.insert(magic);
						++c;
					}
					else {
						size_t r;
						while ((r = rand()) == magic);
						hum.insert(r);
					}
				}
				assert(hum.count(magic) == c);
			}
		}
	}

	static void template_methods() {
		{
			typedef unordered_set<Key, KeyHash, KeyEqual> Container;
			Container cont;
			cont.insert(Key(1));
			cont.insert(Key(2));
			cont.insert(Key(3));
			cont.insert(Key(4));

			assert(cont.count(Key(1)) == 1);
			assert(cont.count(Key(1)) == 1);
			assert(cont.count(Key(5)) == 0);

			assert(cont.find(Key(2))!= cont.end());
			assert(cont.equal_range(Key(2)) != make_pair(cont.begin(), cont.end()));

			Container const& ccont = cont;
			assert(ccont.find(Key(2)) != ccont.end());
			assert(ccont.equal_range(Key(2)) != make_pair(ccont.begin(), ccont.end()));
		}
		{
			typedef unordered_multiset<Key, KeyHash, KeyEqual> Container;
			Container cont;
			cont.insert(Key(1));
			cont.insert(Key(2));
			cont.insert(Key(1));
			cont.insert(Key(2));

			assert(cont.count(Key(1)) == 2);
			assert(cont.count(Key(1)) == 2);
			assert(cont.count(Key(5)) == 0);

			assert(cont.find(Key(2)) != cont.end());
			assert(cont.equal_range(Key(1)) != make_pair(cont.end(), cont.end()));

			Container const& ccont = cont;
			assert(ccont.find(Key(2)) != ccont.end());
			assert(ccont.equal_range(Key(2)) != make_pair(ccont.end(), ccont.end()));
		}
	}

public:
	static void run() {
		uset();
		umultiset();
		umap();
		umultimap();
		user_case();
		hash_policy();
		buckets();
		equal_range();
		template_methods();
	}
};

#endif // UNORDERED_TEST_H