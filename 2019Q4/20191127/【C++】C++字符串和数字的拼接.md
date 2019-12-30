如果你使用过`python`，你会发现字符串和`int/float/double`便捷的拼接方式；但如果你使用`C++`，可能你每次需要的时候搜索一下才能知道。本文提供两种简单的方式来完成这个功能。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191127](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191127)。

<br/>

## std::to_string()
通过`std::to_string()`将数字类型转换成`std::string`类型，从而可以直接使用`+`完成字符串的拼接。
```c
# include <iostream>

int main(int argc, char const *argv[])
{
  std::string str = "hello " + std::to_string(1);
  std::cout << str << std::endl;

  return 0;
}
```
需要注意的是，**std::string是C++11才有的方法，在g++编译的时候需要指定-std=c++11**。

编译并运行这段程序：
```bash
yngzmiao@yngzmiao-virtual-machine:~/test$ g++ test.cpp -o test -std=c++11
yngzmiao@yngzmiao-virtual-machine:~/test$ ./test 
hello 1
```

同时，如果想要转化为`const char*`的类型，可以使用`c_str()`方法：
```c
std::string str = "hello 1";
str.c_str();
```
如果想要去除const属性，需要使用到`const_cast`：
```c
const char* const_char_str = str.c_str();
std::cout << const_char_str << std::endl;

char* char_str = const_cast<char*>(const_char_str);
std::cout << char_str << std::endl;
```

<br/>

## ostringstream
通过字符串流来完成字符串和数字的拼接，再将字符串流的内容转化为`std::string`的类型。

使用ostringstream之前，需要指定头文件：
```c
# include <iostream>
# include <sstream>

int main(int argc, char const *argv[])
{
  std::ostringstream oss;
  oss << "hello " << 2;
  std::string str = oss.str();
  std::cout << str << std::endl;

  return 0;
}
```
编译并运行这段程序：
```bash
yngzmiao@yngzmiao-virtual-machine:~/test$ g++ test.cpp -o test -std=c++11
yngzmiao@yngzmiao-virtual-machine:~/test$ ./test 
hello 2
```
使用ostringstream，如何来`清空内容`呢？
```c
oss.str("");
```
对`float`小数点也可以进行一些格式化：
```c
oss.setf(std::ios::fixed);				//小数定点化
oss.precision(2);						//小数点后2位
```
需要注意的是，**使用小数点的格式化需要在传入小数之前来完成，否则不会生效**。例如：
```c
oss << "hello " << 3.14159265357;
oss.setf(std::ios::fixed);
oss.precision(2);

oss.setf(std::ios::fixed);
oss.precision(2);
oss << "hello " << 3.14159265357;
```
相比较来说，后者会保留小数点后两位，而前者不会。

<br/>

## 注意点
这两种方式在`linux g++11`版本上编译一般都不会出现问题，但是在`Android NDK`上编译，**第一种方式可能会出现问题，因此此时建议使用第二种方式**。

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">