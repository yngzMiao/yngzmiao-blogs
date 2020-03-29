
在写代码的过程中，有时候需要将指针赋值为空指针，以防止野指针。在C中，都是使用NULL来实现的；在C++中，除了NULL之外，还提供了nullptr来进行定义。那么两者之间有什么区别呢，分别适用于什么类型的场景呢？

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q1/20200329](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q1/20200329)。

<br/>

## NULL在C/C++中的含义
`NULL`是一个宏定义，它的值是一个空指针常量，由实现来进行定义。C语言中常数0和(void*)0都是空指针常量；C++中常数0是，而(void*)0 不是。

那问题来了，为什么C中(void*)0是空指针常量，而C++中不是？

因为**C语言中任何类型的指针都可以(隐式地)转换为void* 型，反过来也行；而C++中void* 型不能隐式地转换为别的类型指针**(例如：int* p = (void*)0，使用C++编译器编译会报错)。

可以查看到NULL的宏定义内容：NULL在C和C++中的定义不同，**C中NULL为（void * )0，而C++中NULL为整数0**。

```c
// C语言中NULL定义
#define NULL (void*)0                //c语言中NULL为void类型的指针，但允许将NULL定义为0

// C++中NULL的定义
#ifndef NULL
#ifdef _cpluscplus                       //用于判定是c++类型还是c类型
#define NULL 0                         //c++中将NULL定义为整数0
#else
#define NULL ((void*)0)             //C语言中NULL为void类型的指针
#endif
#endif
```

那既然C/C++标准中，常数0都可作为空指针常量，为什么不统一使用0？

**可能觉得由于(void * )0更能体现指针的意义，而常数0更多的时候是用作整数**。因此，C语言中NULL定义选择了(void*)0。

因此，如果在C++中如下定义：

```c
int *p = NULL;
```

实际表示将指针p的值赋为0，而C++中当一个指针的值为0时，认为指针为空指针。

但是，如果单纯这样设计，在使用过程中可能会产生一个问题：
```c
#include <iostream>

void f(int) {
  std::cout <<"invoke f(int)" << std::endl;
}

void f(void*) {
  std::cout << "invoke f(void*)" << std::endl;
}

int main(int argc, char *argv[]) {
	f(0);
  f(NULL);

	return 0;
}
```

当实参是NULL的时候，到底表示的是0调用f(int)函数，还是表示指针调用f(void*)呢？绝大多数编译器都是选择调用f(int)函数，当然也有的编译器直接编译出错。

<br/>

## nullptr
为了避免上面的情况出现，C++11引入了一个新关键字`nullptr`(也有的称之为：空指针常量)，它的类型为`std::nullptr_t`。在C++中，**void * 不能隐式地转化为任意类型的指针(可以强制转化)，但空指针常数可以隐式地转换为任意类型的指针类型**。

### nullptr与nullptr_t
在`stddef.h`中有如下的描述：
```c
typedef decltype(nullptr) nullptr_t;
```

1. nullptr_t是一种数据类型，而nullptr是该类型的一个实例。通常情况下，也可以通过nullptr_t类型创建另一个新的实例；
2. 所有定义为nullptr_t类型的数据都是等价的，行为也是完全一致的；
3. std::nullptr_t类型，并不是指针类型，但可以**隐式转换成任意一个指针类型(注意不能转换为非指针类型，强转也不行)**；
4. nullptr_t类型的数据不适用于算术运算表达式。但可以用于关系运算表达式(仅能与nullptr_t类型数据或指针类型数据进行比较，当且仅当关系运算符为==、<=、>=等时)。

### nullptr与NULL的区别
1. **NULL是一个宏定义，C++中通常将其定义为0，编译器一般优先把它当作一个整型常量**(C标准下定义为(void*）0)；
2. nullptr是一个**编译期常量**，其类型为nullptr_t。它既不是整型类型，也不是指针类型；
3. **在模板推导中，nullptr被推导为nullptr_t类型，仍可隐式转为指针。但0或NULL则会被推导为整型类型**；
4. **要避免在整型和指针间进行函数重载。因为NULL会被匹配到整型形参版本的函数，而不是预期的指针版本**。

### nullptr与(void*)0的区别
1. nullptr到任意类型指针的转换是隐式的(尽管nullptr不是指针类型，但仍可当指针使用)；
2. (void*)0只是一个强制转换表达式，其返回void*指针类型，只能经过类型转换到其他指针才能用。

例如：
```c
#include <iostream>

int main(int argc, char *argv[]) {
  void* px = NULL;
  // int* py = (void*)0;         //编译错误，不能隐式将void*转为int*类型
  int* pz = (int*)px;           //void*不能隐式转为int*，必须强制转换！

  int* pi = nullptr;            //ok！nullptr可以隐式转为任何其他指针类型
  void* pv = nullptr;           //ok! nullptr可以隐式转为任何其他指针类型

	return 0;
}
```

<br/>

## 总结

**NULL在C语言中是(void * )0，在C++中却是0。这是因为在C++中void * 类型是不允许隐式转换成其他指针类型的，所以之前C++中用0来代表空指针。但是，在重载整型和指针的情况下，会出现匹配错误的情况。所以，C++11加入了nullptr，可以保证在任何情况下都代表空指针**。

<br/>

## 相关阅读
* [第6课 nullptr_t和nullptr](https://www.cnblogs.com/5iedu/p/11277428.html)
* [C/C++杂记：NULL与0的区别、nullptr的来历](https://www.cnblogs.com/malecrab/p/5569707.html)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">