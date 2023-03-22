# vector 数组

`22 March 2023`

----
动态数组是最常用的容器之一，也是`STL`中的序列式容器。
## 迭代器
由于是内存连续的容器，因此，`vector`的迭代器无需进一步封装，它就是原始指针，即：
```c++
typedef Tp* iterator;
typedef const Tp* const_iterator;
```
`STL`对于`vector`的设计也分为了两个层次，基类是`vector_base`，子类是`vector`。
对于`base`，数据域包含了三个指针，如下：
```c++
	Tp* m_start;
	Tp* m_finish;
	Tp* m_end_of_storage;
```
这三个指针标记了这段连续内存的开始，已用部分和结束位置。从构造函数能够看出三个指针的含义：
```c++
vector_base(size_t n, const Alloc&) :m_start(nullptr), m_finish(nullptr), m_end_of_storage(nullptr) {
		m_start = m_allocator(n);
		m_finish = m_start;
		m_end_of_storage = m_start + n;
	}
```
这里`m_start`试图获取`n`个元素大小的一块内存区域，由于一开始数组中没有元素，因此`m_finish`和`m_start`重合，此时`vector`大小为`0`；而`end_of_storage`指向了数组的最末端。

## vector
`vector`是一般静态数组的升级，它能够实现存储的自动扩容，并且这一过程对程序员是透明的，但扩容的背后是有代价的。这里对于一般的接口，例如`size()`、`empty()`就不列举了，重点关注以下`1`个函数：
> 这里用到了内存相关的一些操作，例如拷贝、填充、初始化等等，这些函数在`stl_uninitialized.h`中（请参考对应章节）。

* `insert` 插入

`vector`提供了一个插入接口，允许用户在指定的任意合法位置进行元素插入，其中`pos`是插入位置。
```c++
void insert_aux(iterator pos, const Tp& value) {
	if (m_finish != m_end_of_storage) {
		construct(m_finish, *(m_finish - 1));
		++m_finish;
		Tp val_copy = value;
		stl::copy_backward(pos, m_finish - 2, m_finish - 1);
		*pos = val_copy;
	}
	else {
		const size_type old = size();
		const size_type len = old != 0 ? (old << 1) : 1;
		iterator start = m_allocator(len);
		iterator finish = stl::uninitialized_copy(m_start, pos, start);
		// insert
		construct(finish, value);
		++finish;
		finish = stl::uninitialized_copy(pos, m_finish, finish);
		// free
		destroy(begin(), end());
		m_deallocator(m_start, m_end_of_storage - m_start);
		m_start = start;
		m_finish = finish;
		m_end_of_storage = m_start + len;
	}
}
```
（1）`[fast path]`

* `[1]` 判断`finish`和`end_of_storage`是否相等，也就是看`vector`是否还有剩余空间可以放置元素。
* `[2]` 如果有，便将最后一个元素拷贝一份副本，`finish`后移；
* `[3]` 此时，需要将`[pos, finish]`向后移动一格；
* `[4]` 最后，将待插入元素填入到`pos`位置。
> 这里相当于把插入点后的元素，从最后一个元素开始，逐个向后移动，腾出插入的`pos`位置。这里可能涉及大量内存拷贝。

（2）`[slow path]`
* `[1]` 若`（1）`不满足，则`vector`需要扩容，由于`vector`要求内存连续，因此只能重新开辟一块更大的内存空间；
* `[2]` 开辟的大小为`const size_type len = old != 0 ? (old << 1) : 1;`，开辟大小为原有大小的**两倍**。
* `[3]` 将`[start, pos)`拷贝到新的内存；
* `[4]` 将待插入元素插入到新内存末尾；
* `[5]` 将`[pos, finish)`拷贝到新内存的末尾；
* `[6]` **释放**原有内存，并调整内部三个指针；

> 插入分为`fast path`和`slow path`，大部分情况走第一个分支，少数情况走第二个分支。但是无论哪一个分支，都面临元素的拷贝，在元素数量非常多，插入位置靠前的情形下，`insert`的效率将非常低。

诸如`push_back`（插入的特例，无需移动元素），`insert`等接口是对`insert_aux`的进一步封装。
其余接口的实现相对简单，这里值得说明的是：

* `reserve(n)`函数的作用是提前预留`n`个大小的空间，本质上改变的是`end_of_storage`，用户看到容器大小没有变化，**但这样可以减少内存的拷贝，建议在使用**；而`resize(n)`本质会修改用户看到的容器的大小，本质上还会移动`finish`指针。

* `clear`函数本质**没有**将内存归还给分配器，它只负责调用这些析构函数，直接将`finish`指针移动到`start`位置，也就是它没有改变`end_of_storage`，容器的容量和原来一致，只是用户看到的大小是`0`！

## 总结
`vector`看似神奇，本质上是有一个动态扩充的过程，将内存的申请和拷贝做了封装；

* 缺点

（1）通常`push_back`、`pop_back`这样的操作的高效的，`insert`和`erase`相对（尤其是随机位置）效率较低，应该减少使用。

（2）每一次内存的扩充，都会面临内存的拷贝，虽然`std::move`这样的语义能够减少大对象拷贝带来的开销，
但是在一开始使用`reserve`预分配一定的空间是推荐的做法。

（3）线程不安全的开销，虽然`STL`中所有容器都不是线程安全的（配器是线程安全的），但是`vector`通常只能使用加锁的方式保障安全，因为在写入数据时，可能出现数据迁移（这也是插入操作可能导致迭代器失效的原因），一些无锁结构难以应用。

相对地，如果是`list`这种数据结构，可以结合`std::atomic`和`memory-order`的控制，实现一些[无锁队列](https://luyuhuang.tech/2022/10/30/lock-free-queue.html)，在一些场景下，可能效率更高。

* 优点

（1）`vector`内存连续，随机访问（`random access`）时间复杂度为`O(1)`，并且对`cache locality`较为友好。

（2）`vector`可以进一步封装为栈`stack`这种数据结构。`STL`的`hashtable`中内含了一个`vector`；`priority_queue`也内含了一个`vector`。其可以很方便地为其他数据结构提供支撑。

（3）大部分时候，`vector`使用相对较多，并且性能较好。