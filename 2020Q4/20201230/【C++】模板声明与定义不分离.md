

一般在写C++相关代码的时候，我们总习惯于将类声明和类实现进行分离。也就是说，类的声明一般写在`.h`文件中，而它的实现一般写在`.cpp`文件中。但是，在模板类中，这个习惯却要恰恰相反。即：**要求模板类的类声明和类实现要都放在头文件，而不能分离**。

本文就对模板的这个奇特习惯进行分析。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q4/20201230](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q4/20201230)。

<br/>

## 分离式编译模式
在进行模板特性的讲解之前，首先需要了解一下C++的`分离式编译模式`。

所谓分离编译模式，就是指：**一个程序或者项目由若干个源文件共同实现，而每个源文件单独编译生成目标文件，最后将所有目标文件连接起来形成单一的可执行文件的过程**。

C/C++组织源代码和生成可执行文件的方式就是分离式编译模式。

简单粗暴的理解就是，一个C++项目分为若干个cpp文件和h文件，每个cpp文件单独编译成每个的目标文件，最终将每个cpp文件连接在一起组成最后的单一的可执行文件。这里最重要的点就是：**编译是相对于每个cpp文件而言的**。

接下去的问题就是，对于编译每个cpp文件的时候，是否都需要每个类的实现？

如果都需要每个类的实现，那么就只能将每个类的实现也都写到h文件中，这样在cpp文件中引入的h文件中，才会有每个类的实现；如果不需要每个类的实现，那么就没有必要将每个类的实现写到h文件中。

C/C++所采用的方法是：**只要给出类的声明，就可以在本源文件中使用该类**。由于每个源文件都是独立的编译单元，在当前源文件中使用但未在此类的实现，就假设在其他的源文件中实现好了。

<br/>

## 模板声明与定义
### 声明定义不分离
但是，分离式编译模式却驯不服模板。

C++标准要求编译器在实例化模板时，必须在上下文中可以查看到其实现；而反过来，在看到实例化模板之前，编译器对模板的实现是不处理的。原因很简单，编译器怎么会预先知道typename实参是什么呢？因此模板的实例化与实现必须放到同一文件中。

《C++编程思想》说明了原因：

模板定义很特殊。由`template<…> `处理的任何东西都意味着**编译器在当时不为它分配存储空间，它一直处于等待状态直到被一个模板实例告知**。在编译器和连接器的某一处，有一机制能去掉指定模板的多重定义。所以为了容易使用，几乎总是在头文件中放置全部的模板声明和定义。

简单来说：**只有模板实例化时，编译器才会得知T实参是什么。而编译器在处理模板实例化时，不仅仅要看到模板的定义式，还需要模版的实现体**。

为什么需要这样呢？

比如说存在类Rect<T>, 其类定义式写在test.h，类的实现体写在test.cpp中。对于模板来说，编译器在处理test.cpp文件时，编译器无法预知T的实参是什么，所以编译器对其实现是不作处理的。

紧接着在main.cpp中用到了Rect<float>，这个时候会实例化。也就是说，在test.h中会实例出对Rect<float>进行类的声明。但是，由于分离式编译模式，在编译的时候只需要类的声明即可，因此**编译是没有任何问题的**。

但是在链接的过程中，需要找到Rect<float>的实现部分。但是上面也说了，编译是相对于每个cpp文件而言的。在test.cpp的编译的时候，由于不知道T的实参是什么，并没有对其进行处理。因此，Rect<float>的实现自然并没有被编译，**链接也就自然而然地因找不到而出错**。

也就是说，模板如果将类声明和类实现进行分离，那么分离式编译模式会导致在**链接**的时候出现问题。

### 例子
解释清楚了，接下来可以看一个例子：

在test.h文件中，定义模板类Rect<T>：

```c
#include <iostream>

template<typename T>
class Rect {
  public:
    Rect(T l = 0.0f, T t = 0.0f, T r = 0.0f, T b = 0.0f) :
      left_(l), top_(t), right_(r), bottom_(b) {}

    void display();

    T left_;
    T top_;
    T right_;
    T bottom_;
};
```

在test.cpp文件中，定义模板类Rect<T>方法的实现：

```c
#include "test.h"

template<typename T>
void Rect<T>::display() {
  std::cout << left_ << " " << top_ << " " << right_
    << " " << bottom_ << std::endl;
}
```

最终在main.cpp文件中，使用改模板类：

```c
#include <iostream>
#include "test.h"

int main() {
  Rect<float> rect(1.1f, 2.2f, 3.3f, 4.4f);
  rect.display();

  return 0;
}
```

对这三个文件进行编译，最终会报错，报错的内容为：

```
yngzmiao@yngzmiao-virtual-machine:~/test/build$ cmake .. && make
-- The C compiler identification is GNU 4.8.4
-- The CXX compiler identification is GNU 4.8.4
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /home/yngzmiao/test/build
Scanning dependencies of target test
[ 25%] Building CXX object CMakeFiles/test.dir/test.cpp.o
[ 50%] Linking CXX static library libtest.a
[ 50%] Built target test
Scanning dependencies of target main
[ 75%] Building CXX object CMakeFiles/main.dir/main.cpp.o
[100%] Linking CXX executable main
CMakeFiles/main.dir/main.cpp.o：在函数‘main’中：
main.cpp:(.text+0x3c)：对‘Rect<float>::display()’未定义的引用
collect2: error: ld returned 1 exit status
make[2]: *** [main] 错误 1
make[1]: *** [CMakeFiles/main.dir/all] 错误 2
make: *** [all] 错误 2
```

可以看出，改代码在ld的过程中出现了错误，即链接的时候没有找到实现而出错。

如果将模板类的声明和实现不分离，都写在.h文件中。即如下：

```c
#include <iostream>

template<typename T>
class Rect {
  public:
    Rect(T l = 0.0f, T t = 0.0f, T r = 0.0f, T b = 0.0f) :
      left_(l), top_(t), right_(r), bottom_(b) {}

    void display() {
      std::cout << left_ << " " << top_ << " " << right_
        << " " << bottom_ << std::endl;
    }

    T left_;
    T top_;
    T right_;
    T bottom_;
};
```

最终编译运行没有问题。

### 总结
在分离式编译的环境下，编译器编译某一个cpp文件时并不知道另一个cpp文件的存在，也**不会去查找**（当遇到未决符号时它会寄希望于链接器）。

这种模式在没有模板的情况下运行良好，但遇到模板时就傻眼了，因为模板仅在需要的时候才会实例化出来。所以，当编译器只看到模板的声明时，它**不能实例化该模板**，只能创建一个具有外部链接的符号并期待链接器能够将符号的地址决议出来。

然而当实现该模板的cpp文件中没有用到模板的实例时，编译器懒得去实例化，所以，整个工程中就找不到一行模板实例的二进制代码，于是链接器也黔驴技穷了。

<br/>

## 相关阅读
* [c++ 模板类 声明和定义都放在.h文件的原因](https://blog.csdn.net/u010273652/article/details/21568131)
* [C++为什么要求把类声明和类实现进行分离？又为什么要求模板类的类声明和类实现要都放在头文件而不能分离?](https://www.zhihu.com/question/270627626?sort=created)
* [为什么模板函数应该定义在头文件内](https://blog.csdn.net/tang05505622334/article/details/90478013)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">