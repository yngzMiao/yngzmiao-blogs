

有时候在设计数据结构的时候，可能会遇到两个类需要相互引用的情形。比如类A有类型为B的成员，而类B又有类型为A的成员。

那么这种情形下，两个类的设计上需要注意什么呢？

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2021Q1/20210103](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2021Q1/20210103)。

<br/>

## 同一文件
### 尝试方案
将A和B的定义都放在一个文件中，例如：

```c
#include <iostream>

class A {
  public:
    A() {
      aa_ = 'A';
    }
    char aa_;
    B b_;
};

class B {
  public:
    B() {
      bb_ = 'B';
    }
    char bb_;
    A a_;
};

int main() {

  return 0;
}
```

编译这一段代码，编译出错：

```
yngzmiao@yngzmiao-virtual-machine:~/test$ g++ main.cpp -o main --std=c++11
main.cpp:9:5: error: ‘B’ does not name a type
     B b_;
     ^
```

编译的报错提示，B不是一个数据类型。可能你会想，会不会`前置声明`一下就可以了？即将代码修改为：

```c
#include <iostream>

class B;

class A {
  public:
    A() {
      aa_ = 'A';
    }
    char aa_;
    B b_;
};

class B {
  public:
    B() {
      bb_ = 'B';
    }
    char bb_;
    A a_;
};

int main() {

  return 0;
}
```

编译这一段代码，编译仍然出错：

```
yngzmiao@yngzmiao-virtual-machine:~/test$ g++ main.cpp -o main --std=c++11
main.cpp:11:7: error: field ‘b_’ has incomplete type
     B b_;
       ^
```

编译时出现"**field has incomplete type**"，通常的错误原因为：**类或结构体的前向声明只能用来定义指针对象或引用，因为编译到这里时还没有发现定义，不知道该类或者结构的内部成员，没有办法具体的构造一个对象**，所以会报错。

解决办法：**将类成员改成指针就好了**。程序中使用incomplete type实现前置声明，有助与实现数据类型细节的隐藏。

按照这个办法来进行修改，将b_的类型由B的对象修改成指向类型B的指针：

```c
#include <iostream>

class B;

class A {
  public:
    A() {
      aa_ = 'A';
    }
    char aa_;
    B *b_;
};

class B {
  public:
    B() {
      bb_ = 'B';
    }
    char bb_;
    A a_;
};

int main() {
  A tmp1;
  std::cout << tmp1.aa_ << " " << tmp1.b_->bb_ << std::endl;
  B tmp2;
  std::cout << tmp2.bb_ << " " << tmp2.a_.aa_ << std::endl;

  return 0;
}
```

编译这一段代码，编译顺利，没有问题。运行这段代码：

```
yngzmiao@yngzmiao-virtual-machine:~/test$ g++ main.cpp -o main --std=c++11
yngzmiao@yngzmiao-virtual-machine:~/test$ ./main 
A H
B A
```

又有问题了，tmp1.b_->bb_的打印结果为H。这个问题很容易检查出问题：在类A的定义中，定义了指向类型B的指针b_，但是并没有对该指针分配内存空间，当然会有一些奇怪的值打印出来。可以修改为：

```c
#include <iostream>

class B;

class A {
  public:
    A() {
      aa_ = 'A';
      b_ = new B();
    }
    char aa_;
    B *b_;
};

class B {
  public:
    B() {
      bb_ = 'B';
    }
    char bb_;
    A a_;
};

int main() {
  A tmp1;
  std::cout << tmp1.aa_ << " " << tmp1.b_->bb_ << std::endl;
  B tmp2;
  std::cout << tmp2.bb_ << " " << tmp2.a_.aa_ << std::endl;

  return 0;
}
```

编译又出现问题了：

```
yngzmiao@yngzmiao-virtual-machine:~/test$ g++ main.cpp -o main --std=c++11
main.cpp: In constructor ‘A::A()’:
main.cpp:9:18: error: invalid use of incomplete type ‘class B’
       b_ = new B();
                  ^
main.cpp:3:7: error: forward declaration of ‘class B’
 class B;
       ^
```

编译错误的原因还是incomplete type，即类型B的结构还不知道，怎么能new出来呢？

### 最终代码

如果想要获得正确的代码，不能将new的操作放在构造函数中，放在其他地方手动创建即可：

```c
#include <iostream>

class B;

class A {
  public:
    A() {
      aa_ = 'A';
    }
    char aa_;
    B *b_;
};

class B {
  public:
    B() {
      bb_ = 'B';
    }
    char bb_;
    A a_;
};

int main() {
  A tmp1;
  tmp1.b_ = new B();
  std::cout << tmp1.aa_ << " " << tmp1.b_->bb_ << std::endl;
  B tmp2;
  std::cout << tmp2.bb_ << " " << tmp2.a_.aa_ << std::endl;

  return 0;
}
```

编译并运行这段代码：

```
yngzmiao@yngzmiao-virtual-machine:~/test$ g++ main.cpp -o main --std=c++11
yngzmiao@yngzmiao-virtual-machine:~/test$ ./main 
A B
B A
```

### 原因分析

类A和类B相互引用比较麻烦的`根本原因`在于：**定义A的时候，A的里面有B，所以就需要去查看B的占空间大小，但是查看的时候又发现需要知道A的占空间大小，从而造成死循环**。

<br/>

## 不同文件
不同文件指的是：类型A定义在A.h文件，类型B定义在B.h文件，同时在main.cpp中创建A、B类型的对象进行输出。通过上文的一些经验，可能还需要create()函数来对B指针进行new操作。

由于不同文件的写法，坑比较多，接下来就直接给出正确的代码内容。

在A.h文件中，定义类A：

```c
#ifndef A_H
#define A_H

class B;                // highlight 1

class A {
  public:
    A() {
      aa_ = 'A';
    }
    void create();
    char aa_;
    B *b_;
};

#endif
```

在A.cpp文件中，定义类A方法的实现：

```c
#include "B.h"        // highlight 2

void A::create() {
 b_ = new B();
}
```

在B.h文件中，定义类B：

```c
#ifndef B_H
#define B_H

#include "A.h"        // highlight 3

class B {
  public:
    B() {
      bb_ = 'B';
    }
    char bb_;
    A a_;
};

#endif
```

最终在main.cpp文件中，使用类A和类B：

```c
#include "B.h"
#include <iostream>

int main() {
  A tmp1;
  tmp1.create();
  std::cout << tmp1.aa_ << " " << tmp1.b_->bb_ << std::endl;
  B tmp2;
  std::cout << tmp2.bb_ << " " << tmp2.a_.aa_ << std::endl;

  return 0;
}
```

对这一段代码中的坑进行罗列：

1. A.h中包含B.h且B.h中包含A.h，**头文件不能循环include**。需要在定义非指针类的那个.h文件include另一个；而定义指针类的那个.h文件需要使用**前置声明**；
2. **create()函数不能在.h文件中进行定义，因为在该函数中需要进行new操作，而该操作需要又另一个类的完整定义，即需要include。由于第一点原因，只好在.cpp文件中进行方法的实现**。

<br/>

## 总结

两个类相互引用，**一个用对象、include；另一个用指针、前置声明、create手动new。手动new的过程不能在构造函数中进行，同时需要知道另一个类的完整定义（include）**。

注意：本文所举例的部分都**没有对new出来的空间进行delete操作，会引发内存泄漏**。这部分需要读者自行补充。

<br/>

## 相关阅读
* [C++中两个类互相引用的解决方法](https://blog.csdn.net/xiqingnian/article/details/41214539)
* [程序编译是出现"field has incomplete type"问题的解决](https://blog.csdn.net/xiaozhu2hao/article/details/18659991)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">