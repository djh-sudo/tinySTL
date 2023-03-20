# 迭代器（iterator）

`14 March 2023`

----

`STL`的容器可以放入任何“元素”，而不同接口都需要对元素进行操作，为此`STL`抽象出了泛化的指针，也就是迭代器，并对齐进行了详细的分类，对于不同的类别，迭代器的“行为”会有所差异，具体反应在**运算符的重载**上。

## iterator base
总体而言，迭代器分为了`input/output iterator`、`forward iterator`、`bidirectional iterator`和`random access iterator`
在`stl_iterator_base.h`这个头文件中，为每种类型的迭代器都定义了一个`tag`，用于标识类型，同时定义了这些迭代器的相关运算，这里以`distance`为例，`distance`定义了两个迭代器之间的距离运算。
* `random access iterator` 这是随机访问的迭代器（类比数组的指针），这类迭代器的特点正如其名，迭代器能够随便游走，因此特化版的`distance`的实现复杂度最低，为`O(1)`。
```c++
template<typename RandomAccessIterator, typename Distance>
inline void distance(RandomAccessIterator first, 
	                 RandomAccessIterator last,
	                 Distance& n,
	                 random_access_iterator_tag) {
	n += (last - first); // O(1)
}
```
只需要将`last`和`first`两个迭代器相减即可（这里自然要求这类迭代器要重载`+=`、`-=`等运算符）
* 其余迭代器
其余迭代器的`distance`实现的复杂度为`O(n)`，代码如下：
```c++
template<typename InputIterator, typename Distance>
inline void distance(InputIterator first, InputIterator last, Distance& n, input_iterator_tag) {
	while (first != last) {
		++first;
		++n;
	} // O(n)
}
```
这里迭代器每次只能逐次往前（可以想象链表的访问），同样需要迭代器重载`!=`、`++`这样的运算符。
不同的迭代器对应不同数据结构的抽象指针。例如：
* `vector`(连续数组)的迭代器是`random access`，可以随机访问任意位置。
* 双向链表`list`的迭代器是`bidrectional`，可以向前或者向后移动。
* 单项链表/哈希表的迭代器是`forward`，只能单向的移动。
## iterator
在`iterator.h`头文件中，主要定义了关于迭代器的`adaptor`（适配器），这里主要是逆向迭代器**reverse iterator**。
逆向迭代器的行为和接口与一般的迭代器相反，在实现上本质是对已有迭代器的封装，这里只有`random access`和`bidrectional`两种迭代器有逆向迭代器。
>`Q`：为什么`forward`类型的迭代器没有逆向迭代器？
>
>`A`：`forward`类型的迭代器只能朝一个方向移动，逆向迭代器违背了这一原则。

# 总结
迭代器本质是指针，对原始指针的一个封装，让它在各个容器的操作上统一便捷。正因为对操作符（尤其是`*`、`++/--`）进行了重载，它才足以“聪明”，其行为与原始指针类似，但对于各种容器都能适配（屏蔽了具体细节）。