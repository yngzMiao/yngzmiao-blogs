C++11之前的初始化语法很乱，有四种初始化方式，而且每种之前甚至不能相互转换。让人有种剪不断，理还乱的感觉。因此，C++11添加了统一初始化的方式，本文将对统一初始化的语法进行详细讲解。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q2/20200404](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q2/20200404)。

<br/>

## 统一初始化
### 几种初始化方式
先来看一下，C++用于的几种初始化的方式，以int为例：

```
小括号：int x(0);             //C++98
等号：int x = 0;              //C++98
大括号：int x{0};             //C++98成功，C++11成功
等号和大括号：int x = {0};    //C++98失败，C++11成功
```

可以看出，C++拥有较多的初始化方式，如此便引申出一种统一初始化的方式。

### 统一初始化方式
`统一初始化`，也叫做`大括号初始化`。顾名思义，是使用大括号进行初始化的方式。例如：

```c
#include <iostream>
#include <vector>
#include <complex>

int main(int argc, char *argv[]) {
  int values[]{1, 2, 3};
  std::vector<int> v{2, 3, 5, 7, 11, 13, 17};
  std::vector<std::string> cities{
    "Beijing", "Nanjing", "Shanghai", "Hangzhou"
  };
  std::complex<double> c{3.0, 4.0};

	return 0;
}
```

其实是利用一个事实：**编译器看到{t1, t2, ..., tn}便会做出一个initializer_list<T>，它关联到一个array<T, n>。调用构造函数的时候，该array内的元素会被编译器分解逐一传给函数。但若函数的参数就是initializer_list<T>，则不会逐一分解，而是直接调用该参数的函数**。

例如：vector类型的cities，由于采用{}进行初始化，会形成一个initializer_list<string>，它会关联到一个array<string, 4>。调用vector构造函数时，发现正好有一个接收initializer_list<string>的参数，于是直接调用构造函数。但是complex类型的c，就没有以initializer_list<T>为参数的构造函数，于是在初始化的时候，只能从array中将元素逐一传递给构造函数进行初始化。

**所有的标准容器的构造函数都有以initializer_list<T>为参数的构造函数**。

<br/>

## initializer_list
### 简单用法
使用`initizlizer_list`的最广泛的使用就是：**不定长度同类型参数的情况**。

```c
#include <iostream>

void print(std::initializer_list<int> vals) {
  for(auto iter = vals.begin(); iter != vals.end(); ++iter)
    std::cout << *iter << std::endl;
}

int main(int argc, char *argv[]) {
  print({1, 2, 3, 4, 5});

	return 0;
}
```

对于initizlizer_list，可以利用`iterator`来对包含的元素进行遍历来完成所需的一些操作。

### 构造函数的匹配
当initizlizer_list做参数与其他参数的函数形成重载关系的时候，如何进行函数的选择呢？

```c
#include <iostream>

class P {
  public:
    P() {
      std::cout << "P" << std::endl;
    }
    P(int a, int b) {
      std::cout << "P(int, int) " << a << " " << b << std::endl;
    }
    P(std::initializer_list<int> vars) {
      std::cout << "P(initializer_list) ";
      for(auto a : vars)
        std::cout << a << " ";
      std::cout << std::endl;
    }
};

int main(int argc, char *argv[]) {
  P p1(1, 2);                     // P(int, int) 1 2
  P p2{1, 2};                     // P(initializer_list) 1 2
  P p3{1, 2, 3};                  // P(initializer_list) 1 2 3
  P p4 = {1, 2};                  // P(initializer_list) 1 2
  P p5{};                         // P

	return 0;
}
```

在这个例子中，p1使用小括号进行初始化，直接调用第一个构造函数，这是没有什么可犹豫的。p2、p3、p4都使用的大括号进行初始化，会形成一个initializer_list<int>，而且**正好存在以该类型为参数的构造函数，直接调用该构造函数直接进行初始化**。p5**尽管使用的是大括号进行初始化，但使用的是空大括号，表示没有参数，会调用无参构造函数**。

如果，P类中仅仅只有第一个构造函数，那么情形又会变成什么呢？

```c
#include <iostream>

class P {
  public:
    P(int a, int b) {
      std::cout << "P(int, int) " << a << " " << b << std::endl;
    }
};

int main(int argc, char *argv[]) {
  P p1(1, 2);                     // P(int, int) 1 2
  P p2{1, 2};                     // P(int, int) 1 2
  P p3{1, 2, 3};                  // Error 出错
  P p4 = {1, 2};                  // P(int, int) 1 2

	return 0;
}
```

此时由于没有以initializer_list<int>类型为参数的构造函数，p2、p4内的元素会将会**被拆解，逐一传递给构造函数进行初始化**。但是，p3由于元素的个数为3个，与构造函数的参数数量不同，不可以调用。

总结下，initializer_list<T>与重载构造函数的关系：
1. 当构造函数形参中不带initializer_list<T>时，小括号和大括号的意义没有区别；
2. 如果构造函数中**带有initializer_list<T>形参，采用大括号初始化语法会强烈优先匹配带有initializer_list<T>形参的重载版本，而其他更精确匹配的版本可能没有机会被匹配**；
3. **空大括号构造一个对象时，表示没有参数(而不是空的initializer_list对象)，因此，会匹配默认的无参构造函数**，而不是匹配initializer_list<T>形参的版本的构造函数；
4. 拷贝构造函数和移动构造函数也可能被带有initializer_list形参的构造函数劫持。

### 源码分析
下面通过对initizlizer_list的源码，分析来探究其深层次的原理：

```c
template<class _E>
class initializer_list
{
public:
  typedef _E value_type;
  typedef const _E& reference;
  typedef const _E& const_reference;
  typedef size_t size_type;
  typedef const _E* iterator;
  typedef const _E* const_iterator;

private:
  iterator _M_array;
  size_type _M_len;

  // The compiler can call a private constructor.
  constexpr initializer_list(const_iterator __a, size_type __l)
  : _M_array(__a), _M_len(__l) { }

public:
  constexpr initializer_list() noexcept
  : _M_array(0), _M_len(0) { }

  constexpr size_type size() const noexcept { return _M_len; }
  constexpr const_iterator begin() const noexcept { return _M_array; }
  constexpr const_iterator end() const noexcept { return begin() + size(); }
};
```

可以看到initializer_list<T>内部存储了两个变量：**_M_array(迭代器变量)和_M_len(长度)**。当调用构造函数的时候，就会将这两个变量进行初始化赋值。那这两个变量是怎么来的呢？

其实，**当用{}进行初始化的时候，首先会创建一个array<T>，并将初始化元素存放起来。然后，调用initializer_list<T>的构造函数，用array<T>首元素的迭代器和array<T>的元素个数，进行初始化**。

如果仔细看会发现，initializer_list<T>构造函数是private类型的，按道理来说，是没有办法外部调用的！但是，在源码中也注明了，编译器可以调用该private构造函数。

除此之外，还有如下几个注意点：
1. initializer_list<T>是一个轻量级的容器类型，内部定义了iterator等容器必需的概念。其中有3个成员接口：size()、begin()和end()。遍历时取得的迭代器是只读的，无法修改其中的某一个元素的值；
2. 对于initializer_list<T>而言，它可以接收任意长度的初始化列表，但要求**元素必须是同种类型T(或可转换为T)**；
3. **Initializer_list<T>内部并不负责保存初始化列表中的元素拷贝，仅仅是列表中元素的引用而己**。因此，**通过过拷贝构造对象与原对象共享列表中的元素空间**。也就是说，**initializer_list<T>的内部并没有内含该array<T>的内容，仅仅是拥有指向array<T>的迭代器**。如果拷贝构造或者拷贝赋值的话，array<T>的内容只有一份，但有两份迭代器指向。如果对initializer_list<T>对象copy一个副本，默认是浅拷贝，此时两个对象指向同一个array<T>。这是危险的。

也就是说，下面的情形是不允许的：

```c
std::initializer_list<int> func(void)
{
  int a = 1, b = 2;
  return {a, b};      //由于initializer_list保存的是对象的引用，但a与b是局部变量在
                      //func返回后会被释放，initializer_list内部会存在空悬指针！危险！
                      //正确的做法可以将返回值改为保存副本的容器，如vector<int>
}

//注意下面s1、 s2、s3和s4均共享元素空间
initializer_list<string> s1 = { "aa", "bb", "cc", "dd" };
initializer_list<string> s2 = s1;
initializer_list<string> s3(s1);
initializer_list<string> s4;
s4 = s1;
```

### 其他
之前版本的C++，min、max只可以进行两个数之间的比较，但是有了initializer_list<T>之后，现在支持如下的比较：
```c
std::max({4, 3, 5, 7});
std::min({4, 3, 5, 7});
```

这是因为，源码中增加了如下的定义：

```c
template<typename _Tp>
inline _Tp max(initializer_list<_Tp> __l)
{ return *std::max_element(__l.begin(), __l.end()); }

template<typename _Tp>
inline _Tp min(initializer_list<_Tp> __l)
{ return *std::min_element(__l.begin(), __l.end()); }
```

<br/>

## 相关阅读
* [第5课 统一初始化(Uniform Initialization)](https://www.cnblogs.com/5iedu/p/11239244.html)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">
