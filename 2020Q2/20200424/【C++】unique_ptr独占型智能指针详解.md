指针是C/C++区别于其他语言的最强大的语法特性，借助指针，C/C++可以直接操纵内存内容。但是，指针的引入也带来了一些使用上的困难，这要求程序员自己必须手动地对分配申请的内存区进行管理。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q2/20200424](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q2/20200424)。

<br/>

## unique_ptr基本用法
`unique_ptr`是C++11提供的用于防止内存泄漏的智能指针中的一种实现，**独享被管理对象指针所有权的智能指针。unique_ptr对象包装一个原始指针，并负责其生命周期。当该对象被销毁时，会在其析构函数中删除关联的原始指针**。

unique_ptr对象**始终是关联的原始指针的唯一所有者，实现了独享所有权的语义**。一个非空的unique_ptr总是拥有它所指向的资源。**转移一个unique_ptr将会把所有权也从源指针转移给目标指针(源指针被置空)。拷贝一个unique_ptr将不被允许**，因为如果你拷贝一个unique_ptr，那么拷贝结束后，这两个unique_ptr都会指向相同的资源，它们都认为自己拥有这块资源(所以都会企图释放)。因此unique_ptr是一个仅能移动的类型。当指针析构时，它所拥有的资源也被销毁。默认情况下，资源的析构是伴随着调用unique_ptr内部的原始指针的`delete`操作的。

unique_ptr具有`->`和`*`运算符重载符，因此它可以像普通指针一样使用。

### 初始化方式
unique_ptr有如下几种初始化方式：
1. **裸指针直接初始化，但不能通过隐式转换来构造**，因为unique_ptr构造函数被声明为explicit；
2. **允许移动构造，但不允许拷贝构造**，因为unique_ptr是个只移动类型；
3. 通过`make_unique`构造，但这是C++14才支持的语法。需要注意的是：make_unique不支持添加删除器，或者初始化列表。

例如：
```c
#include <iostream>
#include <memory>

class Frame {};

int main()
{
  std::unique_ptr<Frame> f(new Frame());              // 裸指针直接初始化
  std::unique_ptr<Frame> f1 = new Frame();            // Error，explicit禁止隐式初始化
  std::unique_ptr<Frame> f2(f);                       // Error，禁止拷贝构造函数
  std::unique_ptr<Frame> f3 = f;                      // Error，禁止拷贝构造函数
  f1 = f;                                             // Error，禁止copy赋值运算符重载

  std::unique_ptr<Frame> f4(std::move(new Frame()));      // 移动构造函数
  std::unique_ptr<Frame> f5 = std::move(new Frame());     // Error，explicit禁止隐式初始化
  std::unique_ptr<Frame> f6(std::move(f4));               // 移动构造函数
  std::unique_ptr<Frame> f7 = std::move(f6);              // move赋值运算符重载

  std::unique_ptr<Frame[]> f8(new Frame[10]());       // 指向数组

  auto f9 = std::make_unique<Frame>();                // std::make_unique来创建，C++14后支持

  return 0;
}
```

需要格外关注，unique_ptr创建数组对象的方法。

了解了这些，运用刚了解的这些特性，试试下面的代码：

```c
#include <iostream>
#include <memory>

class Frame {};

void fun(std::unique_ptr<Frame> f) {}

std::unique_ptr<Frame> getfun() {
  return std::unique_ptr<Frame>(new Frame());       // 右值，被移动构造
                                                    // 就算不是右值，也会被编译器RVO优化掉
}

int main()
{
  std::unique_ptr<Frame> f1(new Frame());
  Frame* f2 = new Frame();
  fun(f1);                    // Error，禁止拷贝构造函数
  fun(f2);                    // Error，explit禁止隐式转换
  fun(std::move(f1));         // 移动构造函数

  std::unique_ptr<Frame> f3 = getfun();       // 移动构造函数

  return 0;
}
```

### 删除器
根据unique_ptr的模板类型来看：
```c
template <typename _Tp, typename _Dp = default_delete<_Tp> >
class unique_ptr {...}
```

模板参数上，前者为unique_ptr需要关联的原始指针的类型，后者为`删除器`，默认值为`default_delete`。也就是说，**删除器是unique_ptr类型的组成部分，可以是普通函数指针或lambda表达式。注意，当指定删除器时需要同时指定其类型，即_Dp不可省略，可通过decltype获得**。

删除器的作用就是规定：**当unique_ptr对象被销毁时，在其析构函数中释放关联的原始指针的方式**。一般情况下，都是通过delete进行释放操作。也就是说，一般情况下，不需要进行指定，使用默认的即可。例如：

```c
#include <iostream>
#include <memory>

class Frame {};

void myDeleter(Frame* p)
{
  std::cout << "invoke deleter(Frame*)"<< std::endl;
  delete p;
}

int main()
{
  std::unique_ptr<Frame, decltype(&myDeleter)> f1(new Frame(), myDeleter);
  auto del = [](Frame* p) {
    std::cout << "invoke deleter([](Frame *))"<< std::endl;
    delete p;
  };
  std::unique_ptr<Frame, decltype(del)> f2(new Frame(), del);

  return 0;
}
```

**使用默认的删除器时，unique_ptr对象和原始指针的大小是一样的**。当自定义删除器时，如果删除器是函数指针，则unique_ptr对象的大小为8字节。对于函数对象的删除器，unique_ptr对象的大小依赖于存储状态的多少，无状态的函数对象(如不捕获变量的lambda表达式)，其大小为4字节。

### 常用操作
* u.get()：返回unique_ptr中保存的裸指针；
* u.reset(...)：重置unique_ptr；
* u.release()：放弃对指针的控制权，返回裸指针，并将unique_ptr自身置空。需要注意，**此函数放弃了控制权但不会释放内存，如果不获取返回值，就丢失了指针，造成内存泄露**；
* u.swap(...)：交换两个unique_ptr所指向的对象。

```c
#include <iostream>
#include <memory>

class Frame {};

int main()
{
  std::unique_ptr<Frame> f1(new Frame());
  Frame* f = f1.get();

  std::unique_ptr<Frame> f2;
  f2.reset(f1.release());
  f2.swap(f1);

  return 0;
}
```

额外需要注意的是：**尽管unique_ptr禁止了拷贝构造和拷贝赋值，但是，nullptr是可以用来赋值的**：
```c
u = nullptr;       //释放u所指向的对象，将u置为空
u.reset(nullptr);    // u置为空
```

### 特点
与auto_ptr相比unique_ptr有如下特点：
1. unique_ptr是一个独享所有权的智能指针，无法进行复制构造、copy赋值操作，只能进行移动操作。无法使两个unique_ptr指向同一个对象；
2. unique_ptr智能指向一个对象，如果当它指向其他对象时，之前所指向的对象会被摧毁；
3. unique_ptr对象会在它们自身被销毁时使用删除器自动删除它们管理的对象；
4. unique_ptr支持创建数组对象方法。

<br/>

## unique_ptr源码剖析
unique_ptr的源码部分分成指向**单个类型对象和指向数组类型两部分**，其中主要源码内容如下：

```c++
// 指向单个类型对象
template <typename _Tp, typename _Dp = default_delete<_Tp> >
class unique_ptr
{
  class _Pointer {};

  typedef std::tuple<typename _Pointer::type, _Dp>  __tuple_type;
  __tuple_type                                      _M_t;

  public:
    typedef typename _Pointer::type   pointer;
    typedef _Tp                       element_type;
    typedef _Dp                       deleter_type;

    constexpr unique_ptr() noexcept : _M_t()
    { static_assert(!is_pointer<deleter_type>::value,
        "constructed with null function pointer deleter"); }

    explicit unique_ptr(pointer __p) noexcept : _M_t(__p, deleter_type())     // 裸指针构造函数，explicit阻止隐式构造
    { static_assert(!is_pointer<deleter_type>::value,
        "constructed with null function pointer deleter"); }

    unique_ptr(unique_ptr&& __u) noexcept                 // 移动构造函数
    : _M_t(__u.release(), std::forward<deleter_type>(__u.get_deleter())) { }

    ~unique_ptr() noexcept                                // 析构函数
    {
      auto& __ptr = std::get<0>(_M_t);
      if (__ptr != nullptr)
        get_deleter()(__ptr);
      __ptr = pointer();
    }

    unique_ptr& operator=(unique_ptr&& __u) noexcept      // move赋值运算符重载
    {
      reset(__u.release());
      get_deleter() = std::forward<deleter_type>(__u.get_deleter());
      return *this;
    }

    typename add_lvalue_reference<element_type>::type operator*() const   // 解引用
    {
      _GLIBCXX_DEBUG_ASSERT(get() != pointer());
      return *get();
    }

    pointer operator->() const noexcept                   // 智能指针->运算符
    {
      _GLIBCXX_DEBUG_ASSERT(get() != pointer());
      return get();
    }

    pointer get() const noexcept                          // 获得裸指针
    { return std::get<0>(_M_t); }

    deleter_type& get_deleter() noexcept                  // 获取删除器
    { return std::get<1>(_M_t); }

    explicit operator bool() const noexcept               // 类型转换函数，用于条件语句，如if(uniptr)之类
    { return get() == pointer() ? false : true; }

    pointer release() noexcept                            // 释放指针
    {
      pointer __p = get();
      std::get<0>(_M_t) = pointer();
      return __p;
    }

    void reset(pointer __p = pointer()) noexcept          // 重置指针
    {
      using std::swap;
      swap(std::get<0>(_M_t), __p);
      if (__p != pointer())
        get_deleter()(__p);
    }

    void swap(unique_ptr& __u) noexcept                   // 交换指针
    {
      using std::swap;
      swap(_M_t, __u._M_t);
    }

    unique_ptr(const unique_ptr&) = delete;               // 禁止拷贝构造函数
    unique_ptr& operator=(const unique_ptr&) = delete;    // 禁止copy赋值运算符重载
};

// 指向数组类型
template<typename _Tp, typename _Dp>
class unique_ptr<_Tp[], _Dp>
{
  ...           // 与上文代码类似，省略

  public:
    typename std::add_lvalue_reference<element_type>::type operator[](size_t __i) const     // 数组[]操作符
    {
      _GLIBCXX_DEBUG_ASSERT(get() != pointer());
      return get()[__i];
    }
```

1. **unique_ptr的构造函数被声明为explicit，禁止隐式类型转换的行为**。可避免将一个普通指针传递给形参为智能指针的函数。假设，如果允许将裸指针传给`void foo(std::unique_ptr<T>)`函数，则在函数结束后会因形参超出作用域，裸指针将被delete的误操作；
2. **unique_ptr的拷贝构造和拷贝赋值均被声明为delete**。因此无法实施拷贝和赋值操作，但可以移动构造和移动赋值；
3. **删除器是unique_ptr类型的一部分。默认为std::default_delete**，内部是通过调用delete来实现；
4. **unique_ptr可以指向数组**，并重载了operator []运算符。

<br/>

## 使用场景
### 工厂函数
作为工厂函数的返回类型：
1. **工厂函数负责在堆上创建对象，但是调用工厂函数的用户才会真正去使用这个对象，并且要负责这个对象生命周期的管理**。所以使用unique_ptr是最好的选择。这正好是std::unique_ptr擅长的地方，因为调用者获得了工厂返回的资源的所有权，当unique_ptr析构时，它会自动销毁所拥有的指针；
2. unique_ptr转为shared_ptr很容易，作为工厂函数本身并不知道用户希望所创建的对象的所有权是专有的还是共享的，返回unique_ptr时调用者可以按照需要做变换。

### PImpl机制
`Pimpl`，英文`pointer to implementation`，即**指向实现的指针**。主要思想是**将私有数据和函数放入一个单独的类中，并保存在一个实现文件中，然后在头文件中对这个类进行前向声明并保存一个指向该实现类的指针**。

也就是说，将曾经放在主类中的数据成员放到实现类中去，然后通过指针间接地访问那些数据成员。此时主类中存在只有声明而没有定义的类型，也叫非完整类型。

Pimpl的优点：
1. **信息隐藏**，将具体类的实现封装到另一个类里面，使用者只能看到一个向前的声明和对应的指针。除非使用者去修改对应的实现，否则，它将无法知道具体的实现，也就无法通过一些非法的方式去访问。从一定程度上防止了封装的泄漏；
2. **降低耦合，包含该类声明的文件也不会因为类实现的改变而重新编译，节约编译时间**。

Pimpl的缺点：
1. **需要手动释放资源**，可以使用unique_ptr来解决这个缺点；
2. 真正执行的操作，需要中间增加一层指针的间接调用，增加开销；通过**间接访问**，增加了阅读代码的难度，程序员书写代码也变得复杂。

<br/>

## 相关阅读
* [第20课 unique_ptr独占型智能指针](https://www.cnblogs.com/5iedu/p/11619357.html)
* [C++程序设计机制——Pimpl机制](https://blog.csdn.net/huangjh2017/article/details/70173005)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">