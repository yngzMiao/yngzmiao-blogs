上篇中，主要讲解了右值引用和移动语义的具体定义和用法。在C++11中几乎所有的容器都实现了移动语义，以方便性能优化。本文以C++11容器中的`insert`方法为例，详细讲解在容器中移动语义是如何提高性能的，同时，在这个过程中STL又解决了什么问题。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q2/20200418](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q2/20200418)。

<br/>

## 测试性能
### MyString类和MyStringNoMove类
创建两个类，其中`MyString`类提供了拷贝构造函数、移动构造函数，而`MyStringNoMove`类只提供了拷贝构造函数，并没有提供移动构造函数。

同时，设置一系列静态成员函数用于记录各构造函数、运算符重载函数、析构函数的调用次数。
```c
class MyString {
  public:
    static size_t DCtor;            // 默认构造函数
    static size_t Ctor;             // 构造函数
    static size_t CCtor;            // 拷贝构造函数
    static size_t CAsgn;            // 拷贝赋值运算符重载
    static size_t MCtor;            // move构造
    static size_t MAsgn;            // move赋值运算符重载
    static size_t Dtor;             // 析构函数
  private:
    char * _data;
    size_t _len;
    void _init_data(const char* s) {
      _data = new char[_len + 1];
      memcpy(_data, s, _len);
      _data[_len] = '\0';
    }
  
  public:
    MyString() : _data(NULL), _len(0) {++DCtor;}
    MyString(const char* p) : _len(strlen(p)) {
      ++Ctor;
      _init_data(p);
    }

    MyString(const MyString& str) : _len(str._len) {
      ++CCtor;
      _init_data(str._data);
    }
    MyString(MyString&& str) noexcept
      : _data(str._data), _len(str._len) {
        ++MCtor;
      str._len = 0;
      str._data = NULL;
    }

    MyString& operator= (const MyString& str) {
      ++CAsgn;
      if(_data != str._data) {
        if(_data)
          delete _data;
        _len = str._len;
        _init_data(str._data);
      }
      return *this;
    }
    MyString& operator= (MyString&& str) noexcept {
      ++MAsgn;
      if(_data != str._data) {
        if(_data)
          delete _data;
        _len = str._len;
        _data = str._data;
        str._len = 0;
        str._data = NULL;
      }
      return *this;
    }

    virtual ~MyString() {
      ++Dtor;
      if(_data)
        delete _data;
    }

    char* get() const {return _data;}
};

size_t MyString::DCtor = 0;
size_t MyString::Ctor = 0;
size_t MyString::CCtor = 0;
size_t MyString::CAsgn = 0;
size_t MyString::MCtor = 0;
size_t MyString::MAsgn = 0;
size_t MyString::Dtor = 0;

class MyStringNoMove {
  public:
    static size_t DCtor;            // 默认构造函数
    static size_t Ctor;             // 构造函数
    static size_t CCtor;            // 拷贝构造函数
    static size_t CAsgn;            // 拷贝赋值
    static size_t MCtor;            // move构造
    static size_t MAsgn;            // move赋值
    static size_t Dtor;             // 析构函数
  private:
    char * _data;
    size_t _len;
    void _init_data(const char* s) {
      _data = new char[_len + 1];
      memcpy(_data, s, _len);
      _data[_len] = '\0';
    }
  
  public:
    MyStringNoMove() : _data(NULL), _len(0) {++DCtor;}
    MyStringNoMove(const char* p) : _len(strlen(p)) {
      ++Ctor;
      _init_data(p);
    }

    MyStringNoMove(const MyStringNoMove& str) : _len(str._len) {
      ++CCtor;
      _init_data(str._data);
    }

    MyStringNoMove& operator= (const MyStringNoMove& str) {
      ++CAsgn;
      if(_data != str._data) {
        if(_data)
          delete _data;
        _len = str._len;
        _init_data(str._data);
      }
      return *this;
    }

    virtual ~MyStringNoMove() {
      ++Dtor;
      if(_data)
        delete _data;
    }

    char* get() const {return _data;}
};

size_t MyStringNoMove::DCtor = 0;
size_t MyStringNoMove::Ctor = 0;
size_t MyStringNoMove::CCtor = 0;
size_t MyStringNoMove::CAsgn = 0;
size_t MyStringNoMove::MCtor = 0;
size_t MyStringNoMove::MAsgn = 0;
size_t MyStringNoMove::Dtor = 0;
```

### test_moveable函数
既然准备测试移动语义带来的性能优化，提供`test_moveable`函数来进行测试。test_moveable函数就是重复创建随机数构造T类型对象，并将其放入到vector中，重复次数为value。具体的代码为：

```c
#include <iostream>

template<typename T>
void output_static_data(const T& myStr) {
  std::cout << typeid(myStr).name() << "--" << std::endl;
  std::cout << "CCtor = " << T::CCtor << " MCtor = " << T::MCtor
            << " CAsgn = " << T::CAsgn << " MAsgn = " << T::MAsgn
            << " Dtor = " << T::Dtor << " Ctor = " << T::Ctor
            << " DCtor = " << T::DCtor << std::endl;
}

template<typename T>
void test_moveable(T t, long value) {
  char buf[10];
  typedef typename std::iterator_traits<typename T::iterator>::value_type Vtype;

  std::chrono::milliseconds time1 = 
    std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch());

  for(long i = 0; i < value; ++i) {
    snprintf(buf, 10, "%d", rand());
    auto iter = t.end();
    t.insert(iter, Vtype(buf));
  }

  std::chrono::milliseconds time2 = 
    std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch());
  std::cout << "construction : " << (time2 - time1).count() << std::endl;
  output_static_data(*(t.begin()));
}

int main(int argc, char *argv[]) 
{
  test_moveable(std::vector<MyString>(), 10000000L);
  test_moveable(std::vector<MyStringNoMove>(), 10000000L);
}
```

代码里比较艰涩的部分是如下这一行：
```c
typedef typename std::iterator_traits<typename T::iterator>::value_type Vtype;
```

详细内容是：**通过迭代器iterator的萃取机来进行类型萃取，获取std::vector的模板参数类型**。

### insert函数
对于容器的`insert`函数，以std::vector为例，有两种定义方式：
```c
iterator insert(const_iterator __position, const value_type& __x);
iterator insert(const_iterator __position, value_type&& __x);
```

这个时候大概就能理解整个流程的过程了。当运行到`t.insert(iter, Vtype(buf))`：

* 如果Vtype的类型是MyStringNoMove，由于Vtype(buf)是一个右值，会调用`insert(..., value_type&& __x)`。但由于MyStringNoMove并没有提供移动构造函数，就会调用拷贝构造函数生成一个对象，并将该对象insert到vector的末尾；
* 如果Vtype的类型是MyString，由于Vtype(buf)是一个右值，会调用`insert(..., value_type&& __x)`。但由于MyString提供了移动构造函数，就会直接调用移动构造函数将临时对象Vtype(buf)的生命周期延长，将该临时对象insert到vector的末尾。

可以看到，对于容器的insert函数而言，**如果模板参数类型没有移动构造函数，将会调用拷贝构造函数进行很多的拷贝操作；但如果模板参数类型有移动构造函数，就会直接调用移动构造函数直接转换了资源的所有权**。性能会提高很多。

<br/>

## 完美转发
### insert过程存在的问题
但是，上文讲述的insert函数的整个流程必须要建立在一个前提上：`insert(..., value_type&& __x)`函数内调用构造函数生成对象的时候，_x必须还需要是个右值。这是什么意思呢？

来看几个简单的例子：
```c
#include <iostream>

void process(int& i) {
  std::cout << "process(&) " << i << std::endl;
}

void process(int&& i) {
  std::cout << "process(&&) " << i << std::endl;
}

void forward(int&& i) {
  std::cout << "forward(&&) " << i << std::endl;
  process(i);
}

int main(int argc, char *argv[]) 
{
  int c = 0;
  process(c);                     // process(&) 0
  process(1);                     // process(&&) 1
  process(std::move(c));          // process(&&) 0

  forward(2);                     // forward(&&) 2  process(&) 2
  forward(std::move(c));          // forward(&&) 0  process(&) 0

  return 0;
}
```

可以看出，前三句的打印输出并没有什么问题，主要是后两句的打印输出。经过forward函数的转发之后，无论是2(纯右值)还是std::move(将亡值)，在接下去**调用process函数的时候，都由右值转化为了左值**！

也就是说，如果不经过什么特殊处理的话，调用`insert(..., value_type&& __x)`函数之后，_x就变成了一个左值，就算MyString有移动构造函数，也没有办法调用到啊。只能继续调用拷贝构造函数了。

为什么右值引用变成了左值？

右值引用独立于左值和右值，意思是右值引用类型的变量可能是左值也可能是右值。这比较拗口。简单地说，**右值引用绑定一个右值，但引用本身也是个变量，这个变量可以是左值也可以是右值**。**但是，任何的函数内部，对形参的直接使用，都是按照左值进行的**。

### 完美转发的引入
完美转发就是`std::forward`，其原型为：

```c
template<typename T>
T&& forward(typename remove_reference<T>::type& param)
{
  return static_cast<T&&>(param);         // 可能会发生引用折叠！
}
```

可以看出，完美转发**必须使用在模板实例化的过程中**！它的原理是利用`万能引用`的特性：**如果被一个左值初始化，它就是一个左值引用；如果它被一个右值初始化，它就是一个右值引用，它是左值引用还是右值引用取决于它的初始化**。

在也就是说，无论param是左值还是右值，都强行转化为与T相同的引用类型。此时，就可以理解了：

```c
template<typename T>
void forward(T&& i) {
  std::cout << "forward(&&) " << i << std::endl;
  process(std::forward<T>(i));
}
```

此时可以理解了，尽管i在forward函数内部，按照左值进行的。但是传递给process函数时，强行又转化为T类型，也就是右值引用的类型。

<br/>

## 性能测试结果
测试性能的程序运行结果如下：
```
yngzmiao@yngzmiao-virtual-machine:~/test$ ./main 
construction : 6595
8MyString--
CCtor = 0 MCtor = 26777215 CAsgn = 0 MAsgn = 0 Dtor = 26777215 Ctor = 10000000 DCtor = 0
construction : 6866
14MyStringNoMove--
CCtor = 26777215 MCtor = 0 CAsgn = 0 MAsgn = 0 Dtor = 26777215 Ctor = 10000000 DCtor = 0
```

可以看出，MyString调用的是移动构造函数，MyStringNoMove调用的是拷贝构造函数。两者之间有差距，但其实也不是特别大。至于为什么数量是26777215超过了10000000，因为**vector的扩容复制操作**。

但如果对vector容器进行拷贝构造函数和移动构造函数的性能测试，在test_moveable添加如下代码：

```c
template<typename T>
void test_moveable(T t, long value) {
  ...

  T t1(t);
  std::chrono::milliseconds time3 = 
    std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch());
  std::cout << "copy : " << (time3 - time2).count() << std::endl;
  T t2(std::move(t));
  std::chrono::milliseconds time4 = 
    std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch());
  std::cout << "move : " << (time4 - time3).count() << std::endl;
}
```

编译并运行后的打印结果为：
```
copy : 1364
move : 0
```
可以看出，移动操作比拷贝操作快得多！

本文最后也提一下：某些容器类型的移动操作未必比拷贝操作更快。如：
* **标准库大部分容器类(如vector)，内部是将其元素存放在堆上，然后用指针指向该堆内存。在进行移动操作时，只是进行指针的拷贝。整个容器内容在常数时间内便可移动完成**；
* **std::array对象缺少这样的一根指针，因为其内容数据是直接存储对象上的。虽然std::array提供移动操作，但其移动和拷贝的速度哪个更快，取决于元素的移动和拷贝速度的比较**。同时std::array移动时需要对每一个元素进行移动，总是需要线性时间；
* 许多std::string类型的实现采用了小型字符串优化(SSO)。当使用SSO后，“小型”字符串(如不超过15个字符)会存储在std::string对象内的某个缓冲区内，即内容直接存储在对象上(而不是堆上)。因此，此时是整个对象的移动，速度并比拷贝更快。

<br/>

## 相关阅读
* [引用折叠和完美转发](https://blog.csdn.net/u014351125/article/details/84502427)
* [第15课 完美转发(std::forward)](https://www.cnblogs.com/5iedu/p/11324772.html)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">