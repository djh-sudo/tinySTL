# 双向链表 list

`22 March 2023`

----
双线链表（`list`）是`STL`容器中较为简单的一个序列容器。
## list_node
每个链表的结点`node`，包含了数据域和指针域，如下：
```c++
struct list_node_base {
	struct list_node_base* m_next;
	struct list_node_base* m_prev;
};

template<class Tp>
struct list_node :public list_node_base {
	Tp m_data;
};
```
由于是双向链表，因此指针域部分需要一前一后两个指针；同时，对于每个结点数据，看似占用的存储为`sizeof(Tp)`，但实际还要多出`8`个字节（`32bit`机器），这些是指针域带来的额外开销，优点是前后两端的插入更加便捷。
## iterator
根据前文分析，双线链表的指针支持两个方向的移动，因此其对应的迭代器属于`bidirectional`类别，可以前后移动，迭代器只需要内含一个`list_node_base`指针即可，便于指针的前后移动。
迭代器需要重载的运算符包括了`++`、`--`、`*`、`->`以及`==`和`!=`。迭代器几乎和裸指针没有区别，这些运算符也只是对其中`list_node_base`的一个简单封装。
## list / list_base
实际的`list`会有一个`list_node<Tp>*`，即内含一个头结点，在初始化的时候，这个头结点的两个指针都会指向自己。这样便于判断`list`是否为空。
```c++
list_base(const allocator_type&) {
		m_node = get_node();
		m_node->m_next = m_node;
		m_node->m_prev = m_node;
	}
```
这里`get_node`是分配器分配相应大小的内存。
在析构部分，需要遍历整个链表，将内存归还。
```c++
void clear() {
		list_node<Tp>* cur = (list_node<Tp>*)m_node->m_next;
		while (cur != m_node) {
			list_node<Tp>* tmp = cur;
			cur = (list_node<Tp>*)cur->m_next;
			destroy(&tmp->m_data);
			put_node(tmp);
		}
		m_node->m_next = m_node;
		m_node->m_prev = m_node;
	}
```
注意`destroy`是调用析构函数，之后才能将这块内存归还给空间管理器（`allocator`）。
> **析构函数**
> 这里关于构造函数和析构函数在`stl_construct.h`，参考上一节。
## list相关操作函数
与`list`相关的操作很多，这里重点梳理相对复杂的几个。对于`begin()`、`empty()`这样的接口，这里就省略了，它们通常只有一两行。
> 注意：`begin()`返回的是第一个元素位置，而`end()`返回的是容器末尾（一个不存在的位置）。
* `insert` 插入

`insert`函数的重载有很多，但是其核心的代码并不多，双向链表的插入比单链表要复杂一些，如下：
```c++
inline iterator insert(iterator position, const Tp& x) {
		node* tmp = create_node(x);
		tmp->m_next = position.m_node;
		tmp->m_prev = position.m_node->m_prev;
		position.m_node->m_prev->m_next = tmp;
		position.m_node->m_prev = tmp;
		return tmp;
	}
```
这里首先会将待插入结点的`prev`和`next`指针放好（入链），再调整插入位置`pos`前后的两个结点的指针，一共四个步骤便能完成插入。

* `erase` 删除

删除和插入操作几乎一样，也分为四个步骤，但这里并没有操作待删除结点，只是将其前后的两个结点链接起来，之后调用析构函数，归还内存。
```c++
		list_node_base* next_node = pos.m_node->m_next;
		list_node_base* prev_node = pos.m_node->m_prev;
		node* n = (node*)pos.m_node;
		prev_node->m_next = next_node;
		next_node->m_prev = prev_node;
		Destroy(&n->m_data);
		put_node(n);
		return iterator((node*)next_node);
```
之后的许多操作，例如批量插入、批量删除都是对上述两个操作的封装。
* `transfer` 移动
这个函数的签名是`void transfer(iterator pos, iterator first, iterator last)`，将区间`[first, last)`的元素，整个剪切到`pos`这个位置上（前），这一操作从数据结构的时间复杂度来说，是`O(1)`，但实际要操作的指针较多，大约可以分为`6`个步骤。
```c++
	if (pos == last) return;
	last.m_node->m_prev->m_next = pos.m_node;
	first.m_node->m_prev->m_next = last.m_node;
	pos.m_node->m_prev->m_next = first.m_node;
	// splice [first, last) into its new position
	list_node_base* tmp = pos.m_node->m_prev;
	pos.m_node->m_prev = last.m_node->m_prev;
	last.m_node->m_prev = first.m_node->m_prev;
	first.m_node->m_prev = tmp;
```
* `merge` 合并

这里`merge`可以实现两个链表的**有序**合并，因此其中的元素必须是**可序的**（可比较大小）。对于基本类型，默认是可以比较大小，对于一般对象，需要重载`<`运算符，否则无法调用`merge`。
```c++
void merge(list& x) {
		iterator first1 = begin();
		iterator last1 = end();
		iterator first2 = x.begin();
		iterator last2 = x.end();
		while (first1 != last1 && first2 != last2) {
			// must override operator<()
			if (*first2 < *first1) {
				iterator next = first2;
				transfer(first1, first2, ++next);
				first2 = next;
			}
			else {
				++first1;
			}
		}
		if (first2 != last2) transfer(last1, first2, last2);
	}
```
实现上，借助了`transfer`，这里会直接操作带合并的链表，因此`l1.merge(l2)`，最后的结果保存在`l1`中，`l2`的数据会失效。
后续切片`splice`等操作是基于`transfer`实现的。

* `sort` 排序

最后一个操作是排序，这里采用的是**两路归并排序**，其开辟了一个静态数组，最多可以支持`2^(64)`数据进行排序（内存允许情况下）。
```c++
	void sort() {
		if (m_node->m_next == m_node || m_node->m_next->m_next == m_node)return;
		list carry;
		list counter[64];
		int fill = 0;
		while (!empty()) {
			int i = 0;
			// 每次拿一个元素放到carry里
			carry.splice(carry.begin(), begin());
			while (i < fill && !counter[i].empty()) {
				counter[i].merge(carry);
				carry.swap(counter[i++]);
			}
			carry.swap(counter[i]);
			if (i == fill) ++fill;
		}
		for (int i = 1; i < fill; ++i) {
			counter[i].merge(counter[i - 1]);
		}
		// assert fill >= 1
		this->swap(counter[fill - 1]);
	}
```
## 总结
* 优点：
`list`双向链表操作相对简单，支持`O(1)`时间复杂度内的首尾端插入，排序时间复杂度是`nlog(n)`，插入删除操作的时间复杂度也是`O(1)`。
* 缺点：
存在额外的开销（例如每个结点需要一前一后两个指针），内存的连续性不好，可能各个结点的内存分布散乱，对`cache locality`不太友好（尤其是数据较大，并且有频繁插入删除后）
* 应用：
可以被封装为一个`stack`或者`queue`，虽然`STL`默认的适配器并没有采用`list`，而是`deque`，可能也考虑了内存开销这一因素。