

最近在review代码的时候发现，使用了空指针调用成员函数，并且成员函数内部有使用到成员变量，居然没有出错。很是奇怪，就用一篇博客把关于空指针调用成员函数相关的内容总结起来。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q4/20201231](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q4/20201231)。

<br/>

## 空指针调用成员函数
### 调用普通成员函数
**如果空指针调用普通成员函数，看该函数体中是否使用到了this指针（是否访问非静态成员变量）。如果使用到了this指针，程序会崩溃；如果没有使用到this指针，程序不会崩溃。当然，如果访问静态成员变量，是不会使用到this指针的**。

例如：
```c
#include <iostream>

class A {
  public:
    A() { a_ = 1; }
    ~A() { }

    int GetValueA() {
      std::cout << "GetValueA()" << std::endl;
      return a_;
    }
    void fun() {
      std::cout << "fun()" << std::endl;
    }

    int a_;
};

int main() {
  A *a = nullptr;
  a->fun();
  a->GetValueA();

  return 0;
}
```

运行这段代码，调用fun()函数正常，调用GetValueA()函数出错。运行结果为：

```
yngzmiao@yngzmiao-virtual-machine:~/test$ g++ main.cpp -o main --std=c++11
yngzmiao@yngzmiao-virtual-machine:~/test$ ./main 
fun()
GetValueA()
段错误 (核心已转储)
```

运行GetValueA()函数，cout语句没有使用到this指针，没有问题顺利打印。但是紧接着return语句使用到了成员变量a_，需要使用到this指针，因此直接出错。

### 调用虚函数
**如果空指针调用虚函数，程序会崩溃，无论函数体中是否使用this指针**。

例如：
```c
#include <iostream>

class A {
  public:
    A() { a_ = 1; }
    ~A() { }

    virtual void fun_v() {
      std::cout << "fun_v()" << std::endl;
    }

    int a_;
};

int main() {
  A *a = nullptr;
  a->fun_v();

  return 0;
}
```

运行这段代码，调用fun_V()函数出错。运行结果为：

```
yngzmiao@yngzmiao-virtual-machine:~/test$ g++ main.cpp -o main --std=c++11
yngzmiao@yngzmiao-virtual-machine:~/test$ ./main 
段错误 (核心已转储)
```

为什么空指针调用虚函数就直接崩溃呢？

这是因为一个类一旦拥有虚函数，那么它就拥有`虚函数表`。该类的对象指针为实现多态，会有一个指向该虚函数表的指针（即`虚表指针vfptr`）。当空指针调用虚函数时，会因为使用到虚表指针而直接出错。

### 调用静态成员函数
**如果空指针调用静态成员函数，由于静态成员函数内部没有this指针，也不能调用非静态成员变量，因此程序没有问题**。

### “特例”
如果空指针调用普通成员函数，而且该普通成员函数中有非静态成员变量的出现，一定会出错么？可以看一下下例：

```c
#include <iostream>

class A {
  public:
    A() { a_ = 1; }
    ~A() { }

    int &GetRefA() {
      std::cout << "GetRefA()" << std::endl;
      return a_;
    }

    int a_;
};

int main() {
  A *a = nullptr;
  a->GetRefA();

  return 0;
}
```

运行这段代码，其结果为：

```
yngzmiao@yngzmiao-virtual-machine:~/test$ g++ main.cpp -o main --std=c++11
yngzmiao@yngzmiao-virtual-machine:~/test$ ./main 
GetRefA()
```

运行结果居然没有错误！和上文空指针调用普通成员函数相比，唯一的不同就是返回的是一个引用。**修改成引用之后，居然就没有错误**了。

是不是上文讲解的内容有误呢？并不是。既然是引用，我们可以进行输出或者赋值：

```c
std::cout << a->GetRefA() << std::endl;

a->GetRefA() = 2;
```

上述的两种方式都是有错误的。仔细看来，其实并不违背上文的讲解。**由于返回的是引用，尽管使用了this指针，但是并没有对该指针指向的内容进行读取或者修改操作。一旦需要对内容进行读取或者修改，就会出错**。因此，也不算是特例。

<br/>

## C++的静态绑定
为什么对this指针的读取和修改，会引起空指针的不同表现？这需要对C++的静态绑定讲起。

**对于非虚成员函数，Ｃ++这门语言是静态绑定的**。

```c
a->fun()
```

这语句的意图是：调用对象a的fun()成员函数。

如果这句话在Java或Python等动态绑定的语言之中，编译器生成的代码大概是：找到a的fun()成员函数，调用它（注意，这里的找到是程序运行的时候才找的，这也是所谓动态绑定的含义：**运行时才绑定这个函数名与其对应的实际代码**。有些地方也称这种机制为迟绑定，晚绑定）。

但是对于C++。为了保证程序的运行时效率，Ｃ++的设计者认为凡是编译时能确定的事情，就不要拖到运行时再查找了。所以C++的编译器看到这句话会这么干：

1. 查找a的类型，发现它有一个非虚的成员函数叫fun（编译器干的）；
2. 该函数找到了，在这里生成一个函数调用，直接调A::fun(a)。

所以到了运行时，由于fun()函数里面，若没有任何需要解引用a指针的代码，所以真实情况下也不会出错。**这里对成员函数的解析，和查找其对应的代码的工作都是在编译阶段完成而非运行时完成的，这就是所谓的静态绑定**，也叫早绑定。

更为简单粗暴地理解就是：

```c
a->fun();

fun(A* this);
```

对于上面的两种方式，本质上是没有区别的。**类的成员函数隐藏了一个默认实参：this指针**。

如果在该函数内部需要用到this指针，此时this指针有视空指针，当然出错。如果没有使用到，只是传了一个空指针而已，没有问题。

对于虚函数，Ｃ++这门语言是静态绑定或者静态绑定的。如果使用了多态，就是动态绑定的；如果没有使用多态，就是静态绑定。如果是动态绑定，肯定是需要this指针的；如果是静态绑定，就和上文的解释一致。

<br/>

## 总结
空指针调用成员函数，会分一下几种情况：
* **如果调用编译器确定函数（普通成员函数、静态成员函数），该成员函数中需要对this指针指向的内容进行读取或者修改，出错；反之无错**；
* **如果调用运行期确定函数（使用多态的虚函数），出错**。

<br/>

## 相关阅读
* [是否可以使用空对象指针调用成员函数及访问成员变量](https://www.cnblogs.com/HelloGreen/p/11522263.html)
* [C++中的静态绑定和动态绑定](https://www.cnblogs.com/lizhenghn/p/3657717.html)
* [为什么C++调用空指针对象的成员函数可以运行通过？](https://www.zhihu.com/question/23260677)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">