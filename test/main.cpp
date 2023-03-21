#include "list_test.h"
#include "vector_test.h"
#include "set_test.h"
#include "heap_test.h"
#include "map_test.h"
#include "deque_test.h"

int main() {
	ListTest::run();
	VectorTest::run();
	SetTest::run();
	HeapTest::run();
	MapTest::run();
	DequeTest::run();
	return 0;
}