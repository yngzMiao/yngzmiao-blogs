
`weak_ptr`这个指针天生一副小弟的模样，也是在C++11的时候引入的标准库，它的出现完全是为了弥补它老大`shared_ptr`天生有缺陷的问题。

相比于上一代的智能指针`auto_ptr`来说，新进老大`shared_ptr`可以说近乎完美，但是**通过引用计数实现的它，虽然解决了指针独占的问题，但也引来了引用成环的问题**，这种问题靠它自己是没办法解决的，所以在C++11的时候将`shared_ptr`和`weak_ptr`一起引入了标准库，用来解决循环引用的问题。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q2/20200503](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q2/20200503)。

<br/>

## 循环引用
什么是循环引用的问题呢？在shared_ptr的使用过程中，当强引用计数为0是，就会释放所指向的堆内存。那么问题来了，如果和死锁一样，当两个shared_ptr互相引用，那么它们就永远无法被释放了。

例如：

```c
#include <iostream>
#include <memory>

class CB;
class CA {
  public:
    CA() {
      std::cout << "CA()" << std::endl;
    }
    ~CA() {
      std::cout << "~CA()" << std::endl;
    }
    void set_ptr(std::shared_ptr<CB>& ptr) {
      m_ptr_b = ptr;
    }
  private:
    std::shared_ptr<CB> m_ptr_b;
};

class CB {
  public:
    CB() {
      std::cout << "CB()" << std::endl;
    }
    ~CB() {
      std::cout << "~CB()" << std::endl;
    }
    void set_ptr(std::shared_ptr<CA>& ptr) {
      m_ptr_a = ptr;
    }
  private:
    std::shared_ptr<CA> m_ptr_a;
};

int main()
{
  std::shared_ptr<CA> ptr_a(new CA());
  std::shared_ptr<CB> ptr_b(new CB());
  ptr_a->set_ptr(ptr_b);
  ptr_b->set_ptr(ptr_a);
  std::cout << ptr_a.use_count() << " " << ptr_b.use_count() << std::endl;

  return 0;
}
```

编译并运行结果，打印为：
```
yngzmiao@yngzmiao-virtual-machine:~/test$ ./main 
CA()
CB()
2 2
```

对于打印的内容，你可能会觉得很奇怪，为什么析构函数并没有调用呢？

**既然析构函数没有调用，就说明ptr_a和ptr_b两个变量的引用计数都不是0**。下面分析一下例子中的引用情况：

<img src="https://img-blog.csdnimg.cn/20200503151929866.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzM4NDEwNzMw,size_16,color_FFFFFF,t_70#pic_center" width="75%">

起初定义完ptr_a和ptr_b时，只有①、③两条引用，即ptr_a指向CA对象，ptr_b指向CB对象。然后调用函数set_ptr后又增加了②、④两条引用，即CB对象中的m_ptr_a成员变量指向CA对象，CA对象中的m_ptr_b成员变量指向CB对象。

这个时候，指向CA对象的有两个，指向CB对象的也有两个。当main函数运行结束时，对象ptr_a和ptr_b被销毁，也就是①、③两条引用会被断开，但是②、④两条引用依然存在，每一个的引用计数都不为0，结果就导致其指向的内部对象无法析构，造成内存泄漏。

<br/>

## weak_ptr
### 解决循环引用
`weak_ptr`的出现就是**为了解决shared_ptr的循环引用的问题的**。以上文的例子来说，**解决办法就是将两个类中的一个成员变量改为weak_ptr对象**，比如将CB中的成员变量改为weak_ptr对象，即CB类的代码如下：

```c
class CB {
  public:
    CB() {
      std::cout << "CB()" << std::endl;
    }
    ~CB() {
      std::cout << "~CB()" << std::endl;
    }
    void set_ptr(std::shared_ptr<CA>& ptr) {
      m_ptr_a = ptr;
    }
  private:
    std::weak_ptr<CA> m_ptr_a;
};
```

编译并运行结果，打印为：
```
yngzmiao@yngzmiao-virtual-machine:~/test$ ./main 
CA()
CB()
1 2
~CA()
~CB()
```

通过这次结果可以看到，CA和CB的对象都被正常的析构了。修改后例子中的引用关系如下图所示：

<img src="https://img-blog.csdnimg.cn/20200503152016661.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzM4NDEwNzMw,size_16,color_FFFFFF,t_70#pic_center" width="75%">

流程与上一例子大体相似，但是不同的是④这条引用是通过weak_ptr建立的，并不会增加引用计数。也就是说，CA的对象只有一个引用计数，而CB的对象只有2个引用计数，当main函数返回时，对象ptr_a和ptr_b被销毁，也就是①、③两条引用会被断开，此时CA对象的引用计数会减为0，对象被销毁，其内部的m_ptr_b成员变量也会被析构，导致CB对象的引用计数会减为0，对象被销毁，进而解决了引用成环的问题。

如果仔细看代码的话，会觉得很神奇！定义m_ptr_a修改成std::weak_ptr<CA>类型，但是set_ptr函数定义的参数还是std::shared_ptr<CA>类型。这个时候为什么没有报错？`weak_ptr`和`shared_ptr`的联系是什么呢？

### weak_ptr的原理
weak_ptr是为了配合shared_ptr而引入的一种智能指针，**它指向一个由shared_ptr管理的对象而不影响所指对象的生命周期，也就是，将一个weak_ptr绑定到一个shared_ptr不会改变shared_ptr的引用计数。不论是否有weak_ptr指向，一旦最后一个指向对象的shared_ptr被销毁，对象就会被释放。从这个角度看，weak_ptr更像是shared_ptr的一个助手而不是智能指针**。

### 初始化方式
1. **通过shared_ptr直接初始化，也可以通过隐式转换来构造**；
2. **允许移动构造，也允许拷贝构造**。

```c
#include <iostream>
#include <memory>

class Frame {};

int main()
{
  std::shared_ptr<Frame> f(new Frame());
  std::weak_ptr<Frame> f1(f);                     // shared_ptr直接构造
  std::weak_ptr<Frame> f2 = f;                    // 隐式转换
  std::weak_ptr<Frame> f3(f1);                    // 拷贝构造函数
  std::weak_ptr<Frame> f4 = f1;                   // 拷贝构造函数
  std::weak_ptr<Frame> f5;
  f5 = f;                                         // 拷贝赋值函数
  f5 = f2;                                        // 拷贝赋值函数
  std::cout << f.use_count() << std::endl;        // 1

  return 0;
}
```

需要注意，**weak_ptr绑定到一个shared_ptr不会改变shared_ptr的引用计数**。

### 常用操作
* w.user_count()：返回weak_ptr的强引用计数；
* w.reset(...)：重置weak_ptr。

如何判断weak_ptr指向对象是否存在？

**既然weak_ptr并不改变其所共享的shared_ptr实例的引用计数，那就可能存在weak_ptr指向的对象被释放掉这种情况**。这时，就不能使用weak_ptr直接访问对象。那么如何判断weak_ptr指向对象是否存在呢？C++中提供了lock函数来实现该功能。**如果对象存在，lock()函数返回一个指向共享对象的shared_ptr(引用计数会增1)，否则返回一个空shared_ptr。weak_ptr还提供了expired()函数来判断所指对象是否已经被销毁**。

由于weak_ptr并没有重载`operator ->`和`operator *`操作符，因此不可直接通过weak_ptr使用对象，同时也没有提供get函数直接获取裸指针。典型的用法是调用其lock函数来获得shared_ptr示例，进而访问原始对象。

<br/>

## 使用场景
### 共享对象的线程安全问题
例如：线程A和线程B访问一个共享的对象，如果线程A正在析构这个对象的时候，线程B又要调用该共享对象的成员方法，此时可能线程A已经把对象析构完了，线程B再去访问该对象，就会发生不可预期的错误。

```c
#include <iostream>
#include <memory>
#include <thread>

class Test {
  public:
    Test(int id) : m_id(id) {}
    void showID() {
      std::cout << m_id << std::endl;
    }
  private:
    int m_id;
};

void thread1(Test* t) {
  std::this_thread::sleep_for(std::chrono::seconds(2));
  t->showID();                      // 打印结果：0
}

int main()
{
  Test* t = new Test(2);
  std::thread t1(thread1, t);
  delete t;
  t1.join();

  return 0;
}
```

在例子中，由于thread1等待2s，此时，main线程早已经把t对象析构了。打印m_id，自然不能打印出2了。可以通过shared_ptr和weak_ptr来解决共享对象的线程安全问题。

```c
#include <iostream>
#include <memory>
#include <thread>

class Test {
  public:
    Test(int id) : m_id(id) {}
    void showID() {
      std::cout << m_id << std::endl;
    }
  private:
    int m_id;
};

void thread2(std::weak_ptr<Test> t) {
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::shared_ptr<Test> sp = t.lock();
  if(sp)
    sp->showID();                      // 打印结果：2
}

int main()
{
  std::shared_ptr<Test> sp = std::make_shared<Test>(2);
  std::thread t2(thread2, sp);
  t2.join();

  return 0;
}
```

如果想访问对象的方法，先通过t的**lock方法进行提升操作，把weak_ptr提升为shared_ptr强智能指针**。提升过程中，是通过检测它所观察的强智能指针保存的Test对象的引用计数，来判定Test对象是否存活。**ps如果为nullptr，说明Test对象已经析构，不能再访问；如果ps!=nullptr，则可以正常访问Test对象的方法**。

如果设置t2为分离线程t2.detach()，让main主线程结束，sp智能指针析构，进而把Test对象析构，此时showID方法已经不会被调用，因为在thread2方法中，t提升到sp时，lock方法判定Test对象已经析构，提升失败！

### 观察者模式
观察者模式就是，当观察者观察到某事件发生时，需要通知监听者进行事件处理的一种设计模式。

在多数实现中，观察者通常都在另一个独立的线程中，这就涉及到在多线程环境中，共享对象的线程安全问题(解决方法就是使用上文的智能指针)。这是因为在找到监听者并让它处理事件时，其实在多线程环境中，肯定不明确此时监听者对象是否还存活，或是已经在其它线程中被析构了，此时再去通知这样的监听者，肯定是有问题的。

也就是说，**当观察者运行在独立的线程中时，在通知监听者处理该事件时，应该先判断监听者对象是否存活，如果监听者对象已经析构，那么不用通知，并且需要从map表中删除这样的监听者对象**。其中的主要代码为：

```c
// 存储监听者注册的感兴趣的事件
unordered_map<int, list<weak_ptr<Listener>>> listenerMap;

// 观察者观察到事件发生，转发到对该事件感兴趣的监听者
void dispatchMessage(int msgid) {
  auto it = listenerMap.find(msgid);
  if (it != listenerMap.end()) {
    for (auto it1 = it->second.begin(); it1 != it->second.end(); ++it1) {
      shared_ptr<Listener> ps = it1->lock();            // 智能指针的提升操作，用来判断监听者对象是否存活
      if (ps != nullptr) {                              // 监听者对象如果存活，才通知处理事件
        ps->handleMessage(msgid);
      } else {
        it1 = it->second.erase(it1);                    // 监听者对象已经析构，从map中删除这样的监听者对象
      }
    }
  }
}
```

这个想法来源于：一个用C++写的开源网络库，muduo库，作者陈硕。大家可以在网上下载到muduo的源代码，该源码中对于智能指针的应用非常优秀，其中借助shared_ptr和weak_ptr解决了这样一个问题，多线程访问共享对象的线程安全问题。

### 解决循环引用
循环引用，简单来说就是：两个对象互相使用一个shared_ptr成员变量指向对方的会造成循环引用，导致引用计数失效。上文详细讲述了循环引用的错误原因和解决办法。

### 监视this智能指针
在上文讲述shared_ptr的博文中就有讲述到：**enable_shared_from_this中有一个弱指针weak_ptr，这个弱指针能够监视this。在调用shared_from_this这个函数时，这个函数内部实际上是调用weak_ptr的lock方法**。lock()会让shared_ptr指针计数+1，同时返回这个shared_ptr。

<br/>

## 相关阅读
* [C++设计模式 - 观察者Observer模式](https://blog.csdn.net/QIANGWEIYUAN/article/details/88745835)
* [第22课 weak_ptr弱引用智能指针](https://www.cnblogs.com/5iedu/p/11623757.html)
* [智能指针（三）：weak_ptr浅析](https://blog.csdn.net/albertsh/article/details/82286999)
* [深入掌握C++智能指针](https://blog.csdn.net/QIANGWEIYUAN/article/details/88562935)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">