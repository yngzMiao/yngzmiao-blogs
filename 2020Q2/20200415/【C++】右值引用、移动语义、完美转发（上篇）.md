在C++11，引入了右值引用的概念，在此基础上的移动语义在STL容器中使用非常广泛。简单来说，move语义使得你可以用廉价的move赋值替代昂贵的copy赋值，完美转发使得可以将传来的任意参数转发给其他函数。然而，这些新特性的背后是什么深意和原理呢？将从两篇博文中做详细的介绍。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q2/20200415](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q2/20200415)。

<br/>

## 左值右值基础
### 左值和右值
**左值是一般指表达式结束后依然存在的持久化对象，右值指表达式结束时就不再存在的临时对象**。区分左值和右值的便捷方法：**能对表达式取地址、有名字的对象为左值。反之，不能取地址、匿名的对象为右值**。

表达式的值类别必属于左值或者右值，而右值又可以分成`纯右值`、`将亡值`两种：
* 纯右值(prvalue)：**非引用返回的临时变量、运算表达式产生的临时变量、原始字面量和lambda表达式**等；
* 将亡值(xvalue)：**与右值引用相关的表达式，通常指将要被移动的对象**。如，函数返回类型为T&&、std::move的返回值、转换为T&&的类型转换函数的返回值等(注意，这些都是与右值引用相关的表达式)，具体的会在下文介绍。

例如：
```c
#include <iostream>

int getValue() {
  return 1;
}

int main(int argc, char *argv[]) 
{
  int a = 0;
  int b = getValue();

  return 0;
}
```

对于`int a = 0`，a是左值，0是原始字面量右值。对于`int b = getValue()`，b是左值，`getVar()`返回的临时值是右值。这是由于左值a在表达式结束后仍然存在，而`getValue()`返回的临时值在表达式结束后就销毁了，同时左值a可以取地址，而`getValue()`的返回值却不行。这两者的右值都是纯右值。

简单地说，纯右值就是值可以放到赋值运算符右边的值，而且大部分写代码的人都有一种`代码感`，一下子就能反应过来，比如要是写成：

```c
getValue() = 10;
```

将10赋值给函数返回的一个int类型的返回值，想想都不对，平时谁会这么写，肯定只能放在赋值运算符右边，是右值。

### 左值引用和右值引用
在C++中，引用简单点说就是对变量起了一个别名，内部原理什么的都不是本文需要涉及的。引用可以给左值起一个别名，当然也可以给右值起一个别名。**绑定左值的引用就是左值引用，绑定右值的引用就是右值引用**。例如：

```c
int i = 0;
int& j = i;               // 左值引用
int&& k = 0;              // 右值引用
```

在例子中，i是一个变量，可以取地址，是左值；引用j绑定左值i，那么引用j就是左值引用。0是一个原始字面量，不可以取地址，是右值；引用k绑定右值0，那么引用k就是右值引用。

至于为什么定义引用的时候用`&`、`&&`，这就是语法规定，和定义指针用`*`一样。当然，在定义的时候，不能用左值引用去绑定一个右值，也不能用一个右值引用去绑定一个左值。例如，下面的定义是错误的：

```c
int i = 0;
int& j = 0;               // Error
int&& k = i;              // Error
```

总结起来就是，引用可绑定的值类型(设T是个具体类型)：
1. 左值引用(`T&`)：**只能绑定到左值(非const左值)**；
2. 右值引用(`T&&`)：**只能绑定到右值(非const右值)**；
3. 常量左值引用(`const T&`)：**常量左值引用是个万能的引用类型。它既可以绑定到左值也可以绑定到右值**。它像右值引用一样可以延长右值的生命期。不过相比于右值引用所引用的右值，**常量左值引用的右值在它的余生中只能是只读的**；
4. 常量右值引用(const T&&)：可绑定到右值或const右值。一般很少使用，基本没有实际用处。

左值引用没什么可讲的，不是C++11的新内容。下面主要对右值引用进行讲解，例如：

```c
int&& k = getValue();
```

对于`getValue()`产生的临时值，如果不是被右值引用k绑定，在表达式结束之后就销毁了。但，既然被右值引用k绑定后，`getValue()`产生的临时值会被续命，它的生命周期将会通过右值引用得以延续，和变量k的声明周期一样长。

因此，**通过右值引用的声明，右值又重获新生，其生命周期与右值引用类型变量的生命周期一样长，只要该变量还活着，该右值临时量将会一直存活下去**。

### 万能引用
上文讲到，**当T是一个具体的类型时，T&&表示右值引用，只能绑定右值**。

但是，**若T&&在发生自动类型推断的时候，它是未定的引用类型，如果被一个左值初始化，它就是一个左值引用；如果它被一个右值初始化，它就是一个右值引用，它是左值引用还是右值引用取决于它的初始化。因此，也被称为万能引用**。

需要注意，T必须是使用在函数模板形参，且必须发生在`类型推导`的过程中。例如：

```c
#include <iostream>

template<typename T>
void fun(T&& t) {}

int main(int argc, char *argv[]) 
{
  int x = 10;
  fun(10);                // t是右值
  fun(x);                 // t是左值

  return 0;
}
```

为什么万能引用是万能的呢？先理解一下`引用折叠`的概念：

在C++中，引用的引用是非法的。比如：`auto& &rx = x`(注意两个&之间有空格)这种直接定义引用的引用是不合法的，但是**编译器在通过类型别名或模板参数推导等语境中，会间接定义出引用的引用，这时引用会形成折叠**。

注意的是：引用折叠只会发生在**模板实例化、auto类型推导、创建和运用typedef和别名声明、以及decltype语境**中。具体规则为：

* **所有右值引用折叠到右值引用上仍然是一个右值引用**。如T&& &&折叠为T&&。
* **所有的其他引用类型之间的折叠都将变成左值引用**。如T& &, T& &&, T&& &折叠为T&。可见左值引用会传染，沾上一个左值引用就变左值引用了。根本原因：**在一处声明为左值，就说明该对象为持久对象，编译器就必须保证此对象可靠(左值)**。

再次强调，引用折叠发生在模板实例化的过程中！

```c
fun(10);                // int&& &&，推导出右值引用
fun(x);                 // int& &&，推导出左值引用
```

这里就可以用引用折叠解释这一点：
* 当万能引用(T&&)绑定到左值时，T会被推导为T&类型。从而参数类型为`T& &&`，引用折叠后的类型为T&，左值引用；
* 当万能引用(T&&)绑定到右值时，T会被推导为T&&类型。从而参数类型为`T&& &&`，引用折叠后的类型为T&&，右值引用。

**万能引用就是利用模板推导和引用折叠的相关规则，生成不同的实例化模板来接收传进来的参数**。

<br/>

## 移动语义
### 移动语义的由来
在C++的学习过程中，知道如果是一个带有堆内存的类，必须提供一个深拷贝拷贝构造函数，因为默认的拷贝构造函数是浅拷贝，会发生`指针悬挂`的问题。例如：

```c
#include <iostream>
#include <string.h>

class MyStringNoDeep {
  private:
    char * _data;
    size_t _len;
    void _init_data(const char* s) {
      _data = new char[_len + 1];
      memcpy(_data, s, _len);
      _data[_len] = '\0';
    }
  
  public:
    MyStringNoDeep() : _data(NULL), _len(0) {}
    MyStringNoDeep(const char* p) : _len(strlen(p)) {
      _init_data(p);
    }

    virtual ~MyStringNoDeep() {
      delete _data;
    }

    char* get() const {return _data;}
};    

int main(int argc, char *argv[]) 
{
  char* buf = "Hello World";
  MyStringNoDeep s = MyStringNoDeep(buf);
  std::cout << s.get() << std::endl;

  return 0;
}
```

编译并运行上面的代码，会发现报错。原因是：内部的data指针将会被删除两次，一次是临时右值`MyStringNoDeep(buf)`析构的时候删除一次，第二次拷贝构造函数生成的s对象释放时删除一次，而这**两个对象的data指向同一块内存地址**，这就是所谓的指针悬挂问题。

如果没有报错，是因为需要增加编译选项`-fno-elide-constructors`。这是因为编译器会进行`RVO优化`，RVO(C++的返回值优化)是指：**C++标准允许一种(编译器)实现省略创建一个只是为了初始化另一个同类型对象的临时对象。基本手段是直接将返回的对象构造在调用者栈帧上，这样调用者就可以直接访问这个对象而不必复制**。如此就只要调用一次析构函数，就不会有问题了。

关于RVO的问题，可以查看博文：[【C++】C++函数需要有返回值，但非全分支return（RVO）](https://blog.csdn.net/qq_38410730/article/details/102560872)。

此时添加上深拷贝的拷贝构造函数，即可避免这个问题：

```c
#include <iostream>
#include <string.h>

class MyStringWithDeep {
  private:
    char * _data;
    size_t _len;
    void _init_data(const char* s) {
      _data = new char[_len + 1];
      memcpy(_data, s, _len);
      _data[_len] = '\0';
    }
  
  public:
    MyStringWithDeep() : _data(NULL), _len(0) {}
    MyStringWithDeep(const char* p) : _len(strlen(p)) {
      _init_data(p);
    }

    MyStringWithDeep(const MyStringWithDeep& str) : _len(str._len) {
      _init_data(str._data);
    }

    virtual ~MyStringWithDeep() {
      delete _data;
    }

    char* get() const {return _data;}
};

int main(int argc, char *argv[]) 
{
  char* buf = "Hello World";
  MyStringWithDeep s = MyStringWithDeep(buf);
  std::cout << s.get() << std::endl;

  return 0;
}
```

提供深拷贝的拷贝构造函数虽然可以保证正确，但是在有些时候会造成额外的性能损耗，因为有时候这种深拷贝是不必要的。

比如：`MyStringWithDeep(buf)`会返回临时变量，然后通过这个临时变量拷贝构造了一个新的对象s，临时变量在拷贝构造完成之后就销毁了，如果堆内存很大的话，那么，这个拷贝构造的代价会很大，带来了额外的性能损失。每次都会产生临时变量并造成额外的性能损失，有没有办法避免临时变量造成的性能损失呢？答案是肯定的，C++11已经有了解决方法。

**其实避免性能损失的思路很简单，既然临时变量就已经有了，为什么一定总要拷贝构造一个新的对象s，而不能把临时变量的生命周期变长，直接拿这个临时变量呢？这是不是立即就想到了上面的右值引用的特性**。

### 移动语义
```c
#include <iostream>
#include <string.h>

class MyString {
  private:
    char * _data;
    size_t _len;
    void _init_data(const char* s) {
      _data = new char[_len + 1];
      memcpy(_data, s, _len);
      _data[_len] = '\0';
    }
  
  public:
    MyString() : _data(NULL), _len(0) {}
    MyString(const char* p) : _len(strlen(p)) {
      _init_data(p);
    }

    MyString(const MyString& str) : _len(str._len) {
      std::cout << "MyString(&)" << std::endl;
      _init_data(str._data);
    }
    MyString(MyString&& str) noexcept
      : _data(str._data), _len(str._len) {
      std::cout << "MyString(&&)" << std::endl;
      str._len = 0;
      str._data = NULL;
    }

    virtual ~MyString() {
      if(_data)
        delete _data;
    }

    char* get() const {return _data;}
};

int main(int argc, char *argv[]) 
{
  char* buf = "Hello World";
  MyString s = MyString(buf);
  std::cout << s.get() << std::endl;

  return 0;
}
```

编译并运行这段代码，打印结果为：
```
yngzmiao@yngzmiao-virtual-machine:~/test$ ./main 
MyString(&&)
Hello World
```

观察代码的区别，发现只多了一个**接收右值引用的构造函数(称之为移动构造函数)**。根据上文讲到的引用可绑定的值类型，`MyString(buf)`属于右值，`MyString(const MyString& str)`和`MyString(MyString&& str)`，两个构造函数都可以接收右值。从输出的结果表明，并没有调用拷贝构造函数，而是调用了移动构造函数。

这就是所谓的移动语义，右值引用的一个重要作用是用来支持移动语义的。

### 移动构造函数的写法
观察一下上文移动构造函数的写法：

```c
MyString(MyString&& str) noexcept
  : _data(str._data), _len(str._len) {
  std::cout << "MyString(&&)" << std::endl;
  str._len = 0;
  str._data = NULL;
}
```

可以发现移动构造函数并没有做深拷贝，仅仅是**将指针的所有者转移到了另外一个对象，同时，将参数对象str的指针置为空**。这里仅仅是做了浅拷贝，因此，这个构造函数避免了临时变量的深拷贝问题，从而解决了前面提到的临时变量拷贝构造产生的性能损失的问题。

关于移动构造函数和拷贝构造函数的区别，如下图：

<img src="https://img-blog.csdnimg.cn/2020041417500271.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzM4NDEwNzMw,size_16,color_FFFFFF,t_70#pic_center" width="75%">

* 拷贝构造函数：**将SrcObj对象拷贝到DestObj对象，需要同时将Resourse资源也拷贝到DestObj对象去**。这涉及到内存的拷贝。
* 移动构造函数：通过**偷内存的方式，将资源的所有权从一个对象转移到另一个对象上。但只是转移，并没有内存的拷贝**。可见Resource的所有权只是从SrcObj对象转移到DestObj对象，由于不存在内存拷贝，其效率一般要高于复制构造。

需要注意的一个细节是，提供移动构造函数的同时也会提供一个拷贝构造函数，以防止移动不成功的时候还能拷贝构造，使代码更安全。

移动语义是通过右值引用来匹配临时值的，那么，普通的左值是否也能借助移动语义来优化性能呢，那该怎么做呢？事实上C++11为了解决这个问题，提供了`std::move`方法来将左值转换为右值，从而方便应用移动语义。即：

```c
char* buf = "Hello World";
MyString s0(buf);
MyString s1(s0);                        // MyString(&)
MyString s2(std::move(s0));             // MyString(&&)
MyString s3 = MyString(buf);            // MyString(&&)
```

如果不用`std::move`，会调用拷贝构造函数，而使用`std::move`几乎没有任何代价，只是转换了资源的所有权。它实际上将左值变成右值引用，然后应用移动语义，调用移动构造函数，就避免了拷贝，提高了程序性能。如果一个对象内部有较大的对内存或者动态数组时，很有必要写move语义的拷贝构造函数和赋值函数，避免无谓的深拷贝，以提高性能。事实上，C++11中所有的容器都实现了移动语义，方便性能优化。

这里也要注意对move语义的误解，**move实际上它并不能移动任何东西，它唯一的功能是将一个左值强制转换为一个右值引用，继而用于移动语义**。std::move的原型为：

```c
template<typename T>
decltype(auto) move(T&& param)          //注意，形参是个引用(万能引用)
{
  using ReturnType = typename remove_reference<T>::type&&;      //去除T自身可能携带的引用
  return static_cast<ReturnType>(param);          //强制转换为右值引用类型
}
```

移动构造函数的注意点：
1. 移动语义一定是要修改临时对象的值，所以声明移动构造时应该形如`Test(Test&&)`，而不能声明为`Test(const Test&&)`；
2. 默认的移动构造函数实际上跟默认的拷贝构造函数一样，都是浅拷贝。通常情况下，必须自定义移动构造函数；
3. **对于移动构造函数来说，抛出异常是很危险的。因为移动语义还没完成，一个异常就抛出来，可能会造成悬挂指针。因此，应尽量通过noexcept声明不抛出异常，而一旦出现异常就可以直接调用std::terminate终止程序**。

解释一下第三点：比如在标准库一些容器操作提供了强异常安全保证，为了兼容C++98的遗留代码在升级到C++11时仍保证正确性。库中用`std::move_if_noexcept`模板来替代move函数。**该函数在类的移动构造函数没有声明noxcept关键字时返回一个左值引用从而使变量通过拷贝语义，而在移动构造函数有noexcept时返回一个右值引用，从而使变量可以使用移动语义**。移动操作未加noexcept时，编译器仍会强制调用一个复制操作。

<br/>

## 相关阅读
* [从4行代码看右值引用](https://www.cnblogs.com/qicosmos/p/4283455.html)
* [第13课 右值引用](https://www.cnblogs.com/5iedu/p/11308158.html)
* [第14课 移动语义(std::move)](https://www.cnblogs.com/5iedu/p/11318729.html)
* [第15课 完美转发(std::forward)](https://www.cnblogs.com/5iedu/p/11324772.html)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">