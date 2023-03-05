# 空间配置器（allocator）

`28 Feb 2023 (last day in Feb)`

----

`STL`的分配器是所有容器的的支撑者，它负责全局管理内存，通过阅读`STL/SGI v3.3`源代码中`stl.alloc.h`文件，能够了解空间配置器的细节。

整个空间分配器分为两个级别，一级(`primary`)和二级(`secondary`)分配器。一级分配器用于解决大内存分配(大于`128 bytes`)，而二级分配器负责小内存分配，也是`STL`这部分最核心的思想——内存池。

虽然`malloc`函数的效率相当高，但是有一个缺点，其分配的内存会大于用户所需空间，额外的空间包含了用于记录内存块大小的`cookie`和一些填充以及调试的必要信息，这里关键是`cookie`，这个是不可缺少的部分，具体`cookie`实验可以参考[这里](./Cpp/other/docs/stack-heap.md)。

因此，**频繁的小内存分配**会增加许多额外的存储空间，这对于本就紧张的内存无疑是雪上加霜，因此`STL`实现了一个内存池，用于管理这些小区块。


## 一级分配器
一级分配器实质是对`malloc/free`的包装，在`malloc`失败时，会调用异常处理函数，试图继续
```c++
static void* allocate(size_t __n) {
  void* __result = malloc(__n);
  if (0 == __result) __result = _S_oom_malloc(__n);
  return __result;
}

static void deallocate(void* __p, size_t /* __n */) {
  free(__p);
}
```
> ps. 这里所有的函数和成员变量均为`static`类型。

## 二级分配器
二级分配器是管理小区块的核心，这里的思想是，将区块的大小按照`8`字节为一个等级，切分出从`8`至`128`字节的`16`种区块，每当用户需要小区块时，将需求的大小按照`8`字节向上取整，之后将准备好的对应大小的区块直接拨给用户即可。

只有当内存池不够时，二级分配器会通过一级分配器一次性索要较大的内存（本质调用一次`malloc`），如此以来，极大地减少了`malloc`调用次数，一定程度避免了内存的浪费。但是也有缺点（例如手握了很多小内存块，这些内存块可能完全不连续，无法合并，当有大内存需求时，内存分配会失败）。

我们将源代码做了一定程度的简化，主要有

* 删去了和线程有关的部分
虽然容器做不到线程安全，但是分配器需要能够线程安全的分配内存，这里暂时不考虑线程安全问题。
* 删去了宏编译的部分
实际的`STL`需要兼容各个平台和版本，因此有大量的宏编译信息，用于控制对应平台和版本的代码，但其内容相似，因此只保留了一份代码。
* 简化了其命名
库中的符号命名通常以`__`开头，代码看起来会有有些不习惯，虽然这样做为了减少命名的冲突（事实上有`namespace`兜底），但是在学习时，将这些细节省略了。
### 成员变量
整个二级分配器的所有函数和成员变量都是`static`，这非常有利于修改为`C`的代码。其成员变量有
```c++
	enum {
		ALIGN = 8, MAX_BYTES = 128, NFREELISTS = 16 };
	struct Obj {
		struct Obj* m_free_list_link;
	};
	static Obj* m_free_list[NFREELISTS];
	static char* m_start_free;
	static char* m_end_free;
	static size_t m_heap_size;
```
这里成员变量从后往前看，后三个用于标识内存池的大小，`m_heap_size`是实际总申请的内存数量。这里`Obj`在原有`STL`中是`union`类型，并且有两个成员变量，但实际上只用到这一个，因此这里直接写为了`struct`。

`m_free_list`是存放`16`个块的首地址的数组，这里的所有块都是通过链表串接在一起的，`m_free_list`实际是二级指针（存放地址的地址）。

### 公开接口
这里公开接口只有`3`个，分别是`allocate`、`deallocate`和`reallocate`，用于内存的申请、释放和重置大小。

以`allocate`为例
```c++
static void* allocate(size_t n) {
		void* ret = nullptr;
		if (n <= 0) {
			return nullptr;
		}
		else if (n > (size_t)MAX_BYTES) {
			// primary allocator
			ret = Malloc_Alloc::allocate(n);
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
```
当需要的内存大小大于`128 bytes`时，会直接走一级分配器；
而在`0-128`这个区间内，程序会先计算出它对齐后大小应该落在哪一个位置，即`free_list_index(n)`函数，例如`n=7`时，函数返回`0`，因为`7`字节对齐后是`8`，是最小的一档。
如果对应的`free_list[index]`还有区块，则直接拨给用户；否则调用`refill`函数，去内存池获取区块，然后在切一块给用户。

其余两个成员函数同理。

### 私有成员函数
这里私有成员函数包括了以下`4`个，为公开的成员函数提供必要支持。
```c++
static size_t round_up(size_t bytes);
static size_t free_list_index(size_t bytes);
static void* refill(size_t n);
static char* chunk_alloc(size_t size, int& njobs);
```
* `round_up`，将`bytes`大小向上调整至`8`的倍数
* `free_list_index`，根据`bytes`大小，返回其在`free_list`的对应位置下标。
这里的含义是，根据申请内存大小（将其向上按照`8`取整），观察其对应`index`是多少。

| | | | | | |
| - | - | - | - | - | - |
| 下标`index`|`0`|`1`|`2`|`...`|`15`|
| 大小 |`8`|`16`|`24`|`...`|`128`|

* `refill`，当某个`free_list[index]`没有区块时，需要从内存池分配一块连续的区块，之后切分为相同的大小，按照链表的形式，挂在至对应的`free_list[index]`处，返回单独一个块给用户。

* `chunck_alloc`，实际分配内存的行为、调整内存池等行为由该函数完成。

比较简单的两个函数是`round_up`和`free_list_index`，如下
```c++
static size_t round_up(size_t bytes) {
	return (bytes + (size_t)ALIGN - 1) & (~((size_t)ALIGN - 1));}

static size_t free_list_index(size_t bytes) {
	return (bytes + (size_t)ALIGN - 1) / ((size_t)ALIGN) - 1;}

```

其次是`refill`函数，
```c++
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
```
这里默认一次分配`20`个块，例如用户需要`4`字节，而`free_list[0]`为空，则`refill`函数会首先申请`20 * 8`的空间，之后切割为`8`字节一块，一次串接好。

申请空间的部分由`chunk_alloc`完成，由于空间可能有限，因此不一定能够拿到`20`个块，所以`njobs`以引用的方式传递至`chunk_alloc`，最后会修改为实际的数量，之后将用户需要的一块存放至`result`中，其余部分切割好，放置在`free_list[index]`中。

最后是`chunk_alloc`函数，略微有点长，其实主要分为几种情况。

```C++
static char* chunk_alloc(size_t size, int& njobs) {
	size_t total_bytes = size * njobs;
	size_t left_bytes = m_end_free - m_start_free;
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
			m_start_free = (char*)Malloc_Alloc::allocate(get_bytes);
			// it must succeed, or will throw OOM
		}
		m_heap_size += get_bytes;
		m_end_free = m_start_free + get_bytes;
		return chunk_alloc(size, njobs);
	}
}
```
`m_end_free - m_start_free`表明了内存池的剩余空间大小(字节)

* 1. 如果剩余空间大小足够，则直接分配`size * njobs`给用户。
* 2. 如果剩余空间大于一个`size`，但是小于`njobs`个`size`，则尽可能多的分配给用户。
* 3. 如果不足一个`size`，则首先做好剩余**内存碎片**的管理，将其划给对应的`free_list[index]`，
之后使用`malloc`申请一大批内存（`2 * total_bytes + round_up(m_heap_size >> 4)`），这个是`STL`给出的申请算法，基本是大于所需空间的两倍。
* 4. 如果`(3)`中的`malloc`申请失败，则考虑在`free_list`靠后的位置寻找空闲区块，并划给内存池。（这个时候内存已经相对紧张）
* 5. 如果`(4)`失败，则调用第一级分配器，（大概率抛出`out of memory`异常）。（已经没有多余内存可以分配）

# Reference
- [《`STL`源码剖析》-侯捷](https://github.com/SilverMaple/STLSourceCodeNote/blob/master/STL%E6%BA%90%E7%A0%81%E5%89%96%E6%9E%90%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87%E5%AE%8C%E6%95%B4%E7%89%88(%E6%B8%85%E6%99%B0%E6%89%AB%E6%8F%8F%E5%B8%A6%E7%9B%AE%E5%BD%95).pdf)

- 源代码取自[仓库](https://github.com/steveLauwh/SGI-STL)