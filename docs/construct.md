# 构造与析构
`21 March 2023`

----
构造函数和析构函数是`C++`独有的部分，`STL`在实现的时候，采用模板的方式进行实现，有许多的小技巧。
## 构造函数
```c++
	template<typename T1, typename T2>
	inline void Construct(T1* p, const T2& value) {
		new ((void*)p) T1(value);
	}

	template<typename T1>
	inline void Construct(T1* p) {
		new ((void*)p) T1();
	}
```
构造函数有两种，一种是带有初值的，一种是，默认构造函数，这里本质是调用`new`实现。
然而，诸如`int`、`double`这些基本类型，根本没有必要调用构造函数，也没有真正意义上的构造函数。因此，`STL`对于这些基本类型实现了对应的特化版本的“构造函数”，部分如下：
```c++
inline void Construct(int* p, const int value) { *p = value; }
	inline void Construct(unsigned int* p, const unsigned int value) { *p = value; }
	inline void Construct(char* p, const char value) { *p = value; }
    // ... ...
```
这里可以看到，如果是不带初值的情况，这个函数体直接为空，实际运行时，由于是内联函数，因此不会产生任何代码；若带有初值，构造函数将退化为一条简单赋值语句。
> 这样实现的原因是，减少不必要的初始化开销。
## 析构函数
析构函数类似，如下：
```c++
	template<typename ForwardIterator>
	void destroy_aux(ForwardIterator first, ForwardIterator last, _false_type) {
		for (; first != last; ++first) {
			destroy(&(*first));
		}
	}

	template<class ForwardIterator>
	inline void destroy_aux(ForwardIterator f, ForwardIterator l, _true_type) {}
```
这里看到，第一个析构函数（`aux`是`auxiliary`缩写），如果这是一个一般对象，他需要调用其析构函数，对于一个容器而言，每个元素需要逐个调用析构函数；而对于内置的类型，如`int`，其析构函数没有意义，因此函数体为空。
> 这里`_false_type`和`_true_type`用于区分内置类型和一般类型。

## 总结
构造函数和析构函数部分用模板实现，但是对于内置类型进行了特化，从而获得更好的性能。例如在**百万级**元素的拷贝中，如果是`int`这样的内置类型，无需调用构造函数，能够减少不少开销，若是一般类型（`obj`），对于每个元素都要调用构造函数，其开销相对会大一些。
