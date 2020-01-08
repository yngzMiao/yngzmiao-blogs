当学习`C++`的时候，数组是最基本的结构之一，通常通过以下的方式来定义：
```c
int a[5];
int *b = new int[5];
```
上面一句是在栈上定义了一个长度为`5`的数组，下面一句是在堆上定义了一个长度为`5`的数组，并用一个指针指向它。

在`C++11`中，引入了一种新的数组定义方式`std::array`。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191031](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191031)。

<br/>

## std::array的特性
**std::array是具有固定大小的数组。因此，它并不支持添加或删除元素等改变大小的操作。也就是说，当定义一个array时，除了指定元素类型，还要指定容器大小**。

既然有了内置的数组，为什么还要引入`array`呢？

内置的数组有很多麻烦的地方，比如无法直接对象赋值，无法直接拷贝等等，同时内置的数组又有很多比较难理解的地方，比如数组名是数组的起始地址等等。相比较于如`vector`等容器的操作，内置数组确实有一些不方便的地方。因此，`C++11`就引入`array`容器来代替内置数组。

简单来说，**std::array除了有内置数组支持随机访问、效率高、存储大小固定等特点外，还支持迭代器访问、获取容量、获得原始指针等高级功能。而且它还不会退化成指针给开发人员造成困惑**。

<br/>

## std::array的使用
### 定义
使用`array`之前，需要包含头文件：
```c
# include <array>
```
定义`array`时，需要**指定其数据类型和大小，两者不可或缺**。同时，**array的大小不能使用变量来指定，但对于内置数组来说，是可以使用变量来指定数组大小的**。

定义`array`时，**可以使用{}来直接初始化，也可以使用另外的array来构造，但不可以使用内置数组来构造**。
例如：
```c
# include <iostream>
# include <array>

int main(int argc, char const *argv[])
{
  std::array<int, 5> a0 = {0, 1, 2, 3, 4};          //正确
  std::array<int, 5> a1 = a0;                       //正确
  int m = 5;
  int b[m];                                 //正确，内置数组
  std::array<int, 5> a2;                    //正确
  std::array<int, m> a3;                    //错误，array不可以用变量指定
  std::array<int, 5> a4 = b;                //错误，array不可以用数组指定

  return 0;
}
```
如果使用`gcc`来进行编译，**需要指定c++11标准**：
```bash
g++ test.cpp -o test -std=c++11
./test
```

### 元素访问
`std::array`提供了`[]`、`at`、`front`、`back`、`data`的方式来进行元素：

| 访问方式 | 含义 |
| :---: | :---: |
| at | 访问指定的元素，同时进行越界检查 |
| [] | 访问指定的元素 |
| front | 访问第一个元素 |
| back | 访问最后一个元素 |
| data | 返回指向内存中数组第一个元素的指针 |

和一般的容器一样，`array`还提供了迭代器的方式进行元素遍历和访问：

| 迭代器 | 含义 |
| :---: | :---: |
| begin | 返回指向容器第一个元素的迭代器 |
| end | 返回指向容器尾端的迭代器 |
| rbegin | 返回指向容器最后元素的逆向迭代器 |
| rend | 返回指向前端的逆向迭代器 |

例如：
```c
# include <iostream>
# include <array>

int main(int argc, char const *argv[])
{
  std::array<int, 5> a = {0, 1, 2, 3, 4};
  std::cout << a.front() << " " << a.at(1) << " " << a[2] << " " << *(a.data() + 3) << " " << a.back() << std::endl;

  std::array<int, 5>::iterator iter;
  for (iter = a.begin(); iter != a.end(); ++iter)
    std::cout << *iter << " ";
  std::cout << std::endl;

  std::array<int, 5>::reverse_iterator riter;
  for (riter = a.rbegin(); riter != a.rend(); ++riter)
    std::cout << *riter << " ";
  std::cout << std::endl;

  return 0;
}
```
运行这段代码，输出为：
```bash
yngzmiao@yngzmiao-virtual-machine:~/test$ g++ test.cpp -o test -std=c++11
yngzmiao@yngzmiao-virtual-machine:~/test$ ./test
0 1 2 3 4
0 1 2 3 4 
4 3 2 1 0
```

### 其他函数
`array`支持其它一些函数：
| 函数 | 含义 |
| :---: | :---: |
| empty | 检查容器是否为空 |
| size | 返回容纳的元素数 |
| max_size | 返回可容纳的最大元素数 |
| fill | 以指定值填充容器 |
| swap | 交换内容 |

例如：
```c
#include <iostream>
#include <algorithm>
#include <array>

int main()
{
  std::array<int, 5> a1 = {4, 0, 2, 1, 3};
  std::array<int, 5> a2;

  std::sort(a1.begin(), a1.end());                  //排序函数
  for(int a: a1)
    std::cout << a << ' ';
  std::cout << std::endl;

  std::reverse(a1.begin(), a1.end());                           //反转a1
  for (std::array<int, 5>::iterator iter = a1.begin(); iter != a1.end(); ++iter)
    std::cout << *iter << " ";
  std::cout << std::endl;

  std::reverse_copy(a1.begin(), a1.end(), a2.begin());          //反转a1的内容拷贝到a2
  for (int i = 0; i < a2.size(); ++i)
    std::cout << a2[i] << " ";
  std::cout << std::endl;
}
```
运行这段代码，输出为：
```bash
yngzmiao@yngzmiao-virtual-machine:~/test$ g++ test.cpp -o test -std=c++11
yngzmiao@yngzmiao-virtual-machine:~/test$ ./test
0 1 2 3 4
4 3 2 1 0
0 1 2 3 4 
```
需要注意的是，`std::reverse`和`std::reverse_copy`的区别，**前者反转本身，后者反转本身的内容拷贝到另一个容器中**。

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">
