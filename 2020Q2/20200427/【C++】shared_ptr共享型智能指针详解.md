
指针是C/C++区别于其他语言的最强大的语法特性，借助指针，C/C++可以直接操纵内存内容。但是，指针的引入也带来了一些使用上的困难，这要求程序员自己必须手动地对分配申请的内存区进行管理。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q2/20200427](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q2/20200427)。

<br/>

## shared_ptr基本用法
智能指针的行为类似于常规指针，重要的区别是它负责自动释放所指向的对象。新标准提供的两种重要的智能指针的区别在于管理底层指针的方式：**shared_ptr允许多个指针指向同一个对象；unique_ptr则独占所指向的对象**。

`shared_ptr`使用**引用计数，每一个shared_ptr的拷贝都指向相同的内存。每使用它一次，内部的引用计数加1，每析构一次，内部的引用计数减1，减为0时，释放所指向的堆内存。shared_ptr内部的引用计数是安全的，但是对象的读取需要加锁**。

### 初始化方式
shared_ptr有如下几种初始化方式：
1. **裸指针直接初始化，但不能通过隐式转换来构造**，因为shared_ptr构造函数被声明为explicit；
2. **允许移动构造，也允许拷贝构造**；
3. 通过`make_shared`构造，在C++11版本中就已经支持了。

例如：

```c
#include <iostream>
#include <memory>

class Frame {};

int main()
{
  std::shared_ptr<Frame> f(new Frame());              // 裸指针直接初始化
  std::shared_ptr<Frame> f1 = new Frame();            // Error，explicit禁止隐式初始化
  std::shared_ptr<Frame> f2(f);                       // 拷贝构造函数
  std::shared_ptr<Frame> f3 = f;                      // 拷贝构造函数
  f2 = f;                                             // copy赋值运算符重载
  std::cout << f3.use_count() << " " << f3.unique() << std::endl;

  std::shared_ptr<Frame> f4(std::move(new Frame()));        // 移动构造函数
  std::shared_ptr<Frame> f5 = std::move(new Frame());       // Error，explicit禁止隐式初始化
  std::shared_ptr<Frame> f6(std::move(f4));                 // 移动构造函数
  std::shared_ptr<Frame> f7 = std::move(f6);                // 移动构造函数
  std::cout << f7.use_count() << " " << f7.unique() << std::endl;

  std::shared_ptr<Frame[]> f8(new Frame[10]());             // Error，管理动态数组时，需要指定删除器
  std::shared_ptr<Frame> f9(new Frame[10](), std::default_delete<Frame[]>());

  auto f10 = std::make_shared<Frame>();               // std::make_shared来创建

  return 0;
}
```

可以看出，**shared_ptr和unique_ptr在初始化的方式上就有一些差别：尽管两者都不支持隐式初始化，但是unique_ptr不支持拷贝构造和拷贝赋值，而shared_ptr都支持。除此之外，由于删除器不是shared_ptr类型的组成部分，在管理动态数组的时候，shared_ptr需要指定删除器**。

### 删除器
删除器可以是**普通函数、函数对象和lambda表达式**等。默认的删除器为`std::default_delete`，其内部是通过`delete`来实现功能的。

**与unique_ptr不同，删除器不是shared_ptr类型的组成部分**。也就是说，两个shared_ptr，就算sp1和sp2有着不同的删除器，但只要两者的类型是一致的，都可以被放入vector<shared_ptr<T> >类型的同一容器里。例如：

```c
#include <iostream>
#include <memory>
#include <vector>

class Frame {};

int main()
{
  auto del1 = [](Frame* f){
    std::cout << "delete1" << std::endl;
    delete f;
  };
  auto del2 = [](Frame* f){
    std::cout << "delete2" << std::endl;
    delete f;
  };

  std::shared_ptr<Frame> f1(new Frame(), del1);
  std::shared_ptr<Frame> f2(new Frame(), del2);
  std::unique_ptr<Frame, decltype(del)> f3(new Frame(), del);

  std::vector<std::shared_ptr<Frame> > v;
  v.push_back(f1);
  v.push_back(f2);

  return 0;
}
```

可以很明显地看出，unique_ptr需要指定原指针的指向类型，还需要指定删除器类型；但shared_ptr只需要指定原指针的指向类型即可。

**与std::unique_ptr不同，自定义删除器不会改变std::shared_ptr的大小。其始终是祼指针大小的两倍**。

### 常用操作
* s.get()：返回shared_ptr中保存的裸指针；
* s.reset(...)：重置shared_ptr；
* s.use_count()：返回shared_ptr的**强引用计数**；
* s.unique()：若use_count()为1，返回true，否则返回false。

<br/>

## shared_ptr剖析
### shared_ptr内存模型
shared_ptr的内存模型如下图：

<img src="https://img-blog.csdnimg.cn/20200427132805996.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzM4NDEwNzMw,size_16,color_FFFFFF,t_70#pic_center">

由图可以看出，shared_ptr包含了**一个指向对象的指针和一个指向控制块的指针**。每一个由shared_ptr管理的对象都有一个控制块，它除了**包含强引用计数、弱引用计数之外，还包含了自定义删除器的副本和分配器的副本以及其他附加数据**。

控制块的创建规则：
* **std::make_shared总是创建一个控制块**；
* **从具备所有权的指针出发构造一个std::shared_ptr时，会创建一个控制块**(如std::unique_ptr转为shared_ptr时会创建控制块，因为unique_ptr本身不使用控制块，同时unique_ptr置空)；
* **当std::shared_ptr构造函数使用裸指针作为实参时，会创建一个控制块**。这意味从同一个裸指针出发来构造不止一个std::shared_ptr时会创建多重的控制块，也意味着对象会被析构多次。如果想从一个己经拥有控制块的对象出发创建一个std::shared_ptr，可以传递一个shared_ptr或weak_ptr而非裸指针作为构造函数的实参，这样则不会创建新的控制块。

因此，**尽可能避免将裸指针传递给一个std::shared_ptr的构造函数，常用的替代手法是使用std::make_shared**。如果必须将一个裸指针传递给shared_ptr的构造函数，就直接传递new运算符的结果，而非传递一个裸指针变量。并且，**不要将this指针返回给shared_ptr。当希望将this指针托管给shared_ptr时，类需要继承自std::enable_shared_from_this，并且从shared_from_this()中获得shared_ptr指针**。

也就是说：

```c
#include <iostream>
#include <memory>

class Frame {};

int main()
{
  Frame* f1 = new Frame();
  std::shared_ptr<Frame> f2(f1);
  std::shared_ptr<Frame> f3(f1);          // Error

  return 0;
}
```

尽量不要使用相同的原始指针来创建多个shared_ptr对象，因为在这种情况下，不同的shared_ptr对象不会知道它们与其他shared_ptr对象共享指针。通俗一点解释，就是，f2和f3两个shared_ptr拥有两个控制块，且这两个控制块同时指向f1指针指向的内存区。这是很危险的，极可能出现重复释放空间的情况。

而make_shared没有临时的裸指针，就不会在写法上出现这种情况。

### 尽量使用make函数
GCC编译器中，make_shared内部是通过调用`std::allocate_shared`来实现的：

```c
template<typename _Tp, typename... _Args>
inline shared_ptr<_Tp> make_shared(_Args&&... __args)
{
    typedef typename std::remove_const<_Tp>::type _Tp_nc;
    return std::allocate_shared<_Tp>(std::allocator<_Tp_nc>(),
                   std::forward<_Args>(__args)...);
}
```

与new相比，make系列函数的优势:
1. **避免代码冗余**：创建智能指针时，被创建对象的类型只需写1次。如make_shared<T>()，而用new创建智能指针时，需要写2次；
2. 异常安全：**make系列函数可编写异常安全代码**，改进了new的异常安全性；
3. 提升性能：**编译器有机会利用更简洁的数据结构产生更小更快的代码。使用make_shared<T>时会一次性进行内存分配，该内存单块(single chunck)既保存了T对象又保存与其相关联的控制块。而直接使用new表达式，除了为T分配一次内存，还要为与其关联的控制块再进行一次内存分配**。

make_shared与new方式内存分布对比图：

<img src="https://img-blog.csdnimg.cn/20200427132905301.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzM4NDEwNzMw,size_16,color_FFFFFF,t_70#pic_center">

make系列函数的局限：
1. 所有的make系列函数都**不允许自定义删除器**；
2. make系列函数创建对象时，不能接受{}初始化列表(这是因为完美转发的转发函数是个模板函数，它利用模板类型进行推导。因此无法将{}推导为initializer_list)。换言之，make系列只能将圆括号内的形参完美转发；
3. **自定义内存管理的类(如重载了operator new和operator delete)，不建议使用make_shared来创建**。因为：重载operator new和operator delete时，往往用来分配和释放该类精确尺寸(sizeof(T))的内存块；而make_shared创建的shared_ptr，是一个自定义了分配器(std::allocate_shared)和删除器的智能指针，由allocate_shared分配的内存大小也不等于上述的尺寸，而是在此基础上加上控制块的大小；
4. 对象的内存可能无法及时回收。因为：**make_shared只分配一次内存，减少了内存分配的开销，使得控制块和托管对象在同一内存块上分配**。而**控制块是由shared_ptr和weak_ptr共享的，因此两者共同管理着这个内存块(托管对象+控制块)。当强引用计数为0时，托管对象被析构(即析构函数被调用)，但内存块并未被回收，只有等到最后一个weak_ptr离开作用域时，弱引用也减为0才会释放这块内存块**。原本强引用减为0时就可以释放的内存, 现在变为了强引用和弱引用都减为0时才能释放, 意外的延迟了内存释放的时间。这对于内存要求高的场景来说, 是一个需要注意的问题。

### 引用计数
shared_ptr中的引用计数直接关系到何时是否进行对象的析构，因此它的变动尤其重要。

* shared_ptr的**构造函数会使该引用计数递增，而析构函数会使该计数递减**。但移动构造表示从一个己有的shared_ptr移动构造到一个新的shared_ptr。这意味着一旦新的shared_ptr产生后，原有的shared_ptr会被置空，其结果是引用计数没有变化；
* **拷贝赋值操作同时执行两种操作**(如sp1和sp2是指向不同对象的shared_ptr，则执行`sp1=sp2`时，将修改sp1使得其指向sp2所指的对象。而最初sp1所指向的对象的引用计数递减，同时sp2所指向的对象引用计数递增)；
* reset函数，**如果不带参数时，则引用计数减1。如果带参数时，如sp.reset( p )则sp原来指向的对象引用计数减1，同时sp指向新的对象( p )**；
* 如果实施一次递减后最后的引用计数变成0，即不再有shared_ptr指向该对象，则会被shared_ptr析构掉；
* **引用计数的递增和递减是原子操作，即允许不同线程并发改变引用计数**。

需要注意的是：**shared_ptr的引用计数本身是安全且无锁的，但对象的读写则不是，因为shared_ptr有两个数据成员，读写操作不能原子化**。

<br/>

## this返回shared_ptr
### 问题复现
上文提到：不要将this指针返回给shared_ptr。当希望将this指针托管给shared_ptr时，类需要继承自`std::enable_shared_from_this`，并且从`shared_from_this()`中获得shared_ptr指针。

这里对此进行详细讲解：

```c
#include <iostream>
#include <memory>

class Frame {
  public:
    std::shared_ptr<Frame> GetThis() {
      return std::shared_ptr<Frame>(this);
    }
};

int main()
{
  std::shared_ptr<Frame> f1(new Frame());
  std::shared_ptr<Frame> f2 = f1->GetThis();
  std::cout << f1.use_count() << " " << f2.use_count() << std::endl;

  std::shared_ptr<Frame> f3(new Frame());
  std::shared_ptr<Frame> f4 = f3;
  std::cout << f3.use_count() << " " << f4.use_count() << std::endl;
  
  return 0;
}
```

对于这段代码，打印值是什么呢？编译并运行的结果为：

```
yngzmiao@yngzmiao-virtual-machine:~/test/$ ./main 
1 1
2 2
```

你可能觉得奇怪，GetThis返回的不就是f1本身么，为什么f2和f4的结果就有这样的区别呢？

其实，**直接从this指针创建，会为this对象创建新的控制块！也就相当于从裸指针重新创建一个新的控制块出来**。

### enable_shared_from_this
为了解决这个问题，标准库提供了一个方法：让类派生自一个模板类：`enable_shared_from_this<T>`。然后调用`shared_from_this()`函数即可。

通过**调用shared_from_this()成员函数获得一个和this指针指向相同对象的shared_ptr**。

```c
class Frame : public std::enable_shared_from_this<Frame> {
  public:
    std::shared_ptr<Frame> GetThis() {
      return shared_from_this();
    }
};
```

这样做的原理是什么呢？

```c
template<typename _Tp>
class enable_shared_from_this
{
  protected:
    constexpr enable_shared_from_this() noexcept { }
    enable_shared_from_this(const enable_shared_from_this&) noexcept { }
    enable_shared_from_this& operator=(const enable_shared_from_this&) noexcept { return *this; }
    ~enable_shared_from_this() { }

  public:
    shared_ptr<_Tp> shared_from_this()
    { return shared_ptr<_Tp>(this->_M_weak_this); }

    shared_ptr<const _Tp> shared_from_this() const
    { return shared_ptr<const _Tp>(this->_M_weak_this); }

  private:
    template<typename _Tp1>
    void _M_weak_assign(_Tp1* __p, const __shared_count<>& __n) const noexcept
    { _M_weak_this._M_assign(__p, __n); }

    template<typename _Tp1, typename _Tp2>
    friend void __enable_shared_from_this_helper(const __shared_count<>&,
            const enable_shared_from_this<_Tp1>*,
            const _Tp2*) noexcept;

    mutable weak_ptr<_Tp>  _M_weak_this;
};
```

可以看到：`enable_shared_from_this`模板类提供两个public属性的shared_from_this成员函数。这两个函数内部会通过`_M_weak_this`成员来创建shared_ptr。其中，`_M_weak_assign`函数不能手动调用，这个函数会被shared_ptr自动调用，该函数是用来初始化唯一的成员变量`_M_weak_this`。

现在来分析：根据对象生成顺序，先初始化基类enable_shared_from_this，再初始化派生类Frame对象本身。这时Frame对象己经生成，但_M_weak_this成员还未被初始化，最后应通过`shared_ptr<T> sp(new T())`等方式调用shared_ptr构造函数(内部会调用`_M_weak_assign`)来初始化`_M_weak_this`成员。而如果在调用shared_from_this函数之前weak_this_成员未被初始化，则会通过ASSERT报错提示。

更深层次原理：**这个enable_shared_from_this中有一个弱指针weak_ptr，这个弱指针能够监视this。在调用shared_from_this这个函数时，这个函数内部实际上是调用weak_ptr的lock方法。lock()会让shared_ptr指针计数+1，同时返回这个shared_ptr，这就是工作原理**。

<br/>

## 相关阅读
* [为什么多线程读写 shared_ptr 要加锁？](https://www.cnblogs.com/Solstice/archive/2013/01/28/2879366.html)
* [第23课 优先选用make系列函数](https://www.cnblogs.com/5iedu/p/11625644.html)
* [shared_ptr使用场景、陷阱、性能分析，使用建议](https://blog.csdn.net/INGNIGHT/article/details/99881762)
* [C++ shared_ptr四宗罪(不得不转)](https://www.jianshu.com/p/f1925247c14f)
* [第21课 shared_ptr共享型智能指针](https://www.cnblogs.com/5iedu/p/11622401.html)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">