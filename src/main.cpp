#include <iostream>
#include <unordered_map>
#include <vector>
#include "stl_list.h"
#include "stl_vector.h"
#include "stl_deque.h"

int main() {
// 	srand(NULL);
// 	std::unordered_map<char*, int>mp;
// 	for (int i = 0; i < 3000; ++i) {
// 		int val = rand() % 100;
// 		char* p = (char *)stl::alloc::allocate(val);
// 		mp[p] = val;
// 	}
// 	for (auto& it : mp) {
// 		stl::alloc::deallocate(it.first, it.second);
// 	}
// 	std::cout << "all " << stl::alloc::getHeapSize() << std::endl;
// 	stl::list<std::string>ll;
// 	ll.push_back("123");
// 	ll.push_back("234");
// 	ll.push_back("456");
// 	ll.sort();
// 	std::cout << (ll == ll) << std::endl;
// 	for (auto& it : ll) {
// 		std::cout << it << std::endl;
// 	}
	stl::vector<int>A;
	srand(NULL);
	for (int i = 0; i < 10000; ++i) {
		A.push_back(i);
		A.insert(A.begin(), i);
	}
	
	std::cout << A.size() << std::endl;
	std::cout << A[500] << std::endl;
	A.clear();
	// std::cout << A[0] << std::endl;
	return 0;
}