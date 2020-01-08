说到`cmake`，可能最先想到的就是`CmakeLists.txt`文件，但是在很多情况下，也会看到`.cmake`文件。也许，你会诧异，`.cmake`文件是干什么的，甚至会想`.cmake`文件是不是`cmake`的正统文件，而`CmakeLists.txt`并不是。

但其实，`CmakeLists.txt`才是`cmake`的正统文件，而`.cmake`文件是一个模块文件，可以被`include`到`CMakeLists.txt`中。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191024](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191024)。

<br/>

## include指令
**include指令一般用于语句的复用，也就是说，如果有一些语句需要在很多CMakeLists.txt文件中使用，为避免重复编写，可以将其写在.cmake文件中，然后在需要的CMakeLists.txt文件中进行include操作就行了**。

include指令的结构为：
```cmake
include(<file|module> [OPTIONAL] [RESULT_VARIABLE <var>]
                      [NO_POLICY_SCOPE])
```

虽然，有不少的可选参数，但是一般情况下，都是直接写：
```cmake
include(file|module)
```

注意，为了使`CMakeLists.txt`能够找到该文件，需要指定文件完整路径(绝对路径或相对路径)，当然如果指定了`CMAKE_MODULE_PATH`，就可以直接`include`该目录下的`.cmake`文件了。

`.cmake`文件里面通常是什么信息呢？

**.cmake文件里包含了一些cmake命令和一些宏/函数，当CMakeLists.txt包含该.cmake文件时，当编译运行时，该.cmake里的一些命令就会在该包含处得到执行，并且在包含以后的地方能够调用该.cmake里的一些宏和函数**。

什么是宏？什么是函数？

<br/>

## MACRO宏和function函数
### 宏和函数的定义
先看一下关键字：`cmake`的宏是`MACRO`，函数是`function`。它们的用法是：
```cmake
macro(<name> [arg1 [arg2 [arg3 ...]]])
  COMMAND1(ARGS ...)            # 命令语句
  COMMAND2(ARGS ...)
  ...
endmacro()

function(<name> [arg1 [arg2 [arg3 ...]]])
  COMMAND1(ARGS ...)            # 命令语句
  COMMAND2(ARGS ...)
  ...
function()
```

定义一个名称为`name`的宏（函数），`arg1...`是传入的参数。我们除了**可以用`${arg1}`来引用变量**以外，系统为我们提供了一些特殊的变量：

| 变量 | 说明 |
| :---: |:---: |
| argv# | #是一个下标，0指向第一个参数，累加 |
| argv | 所有的定义时要求传入的参数 |
| argn | 定义时要求传入的参数以外的参数 |
| argc | 传入的实际参数的个数，也就是调用函数是传入的参数个数 |

### 宏和函数的区别
那么宏和函数之间的区别是什么呢？

其实和`C/C++`里面宏和函数之间的区别差不多，**宏就是字符串替换，函数就是使用变量，在命令中途可以对改变量进行修改**。

以`StackOverflow`的例子来了解一下区别：

首先创建一个`CMakeLists.txt`：
```cmake
cmake_minimum_required(VERSION 3.0)
include(test.cmake)
```

在同目录下创建文件`test.cmake`：
```cmake
set(var "ABC")

macro(Moo arg)
  message("arg = ${arg}")
  set(arg "abc")
  message("# After change the value of arg.")
  message("arg = ${arg}")
endmacro()
message("=== Call macro ===")
Moo(${var})

function(Foo arg)
  message("arg = ${arg}")
  set(arg "abc")
  message("# After change the value of arg.")
  message("arg = ${arg}")
endfunction()
message("=== Call function ===")
Foo(${var})
```
运行cmake：
```bash
mkdir build && cd build
cmake ..
```

运行后的输出结果是：
```bash
-- The C compiler identification is GNU 5.4.0
-- The CXX compiler identification is GNU 5.4.0
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
=== Call macro ===
arg = ABC
# After change the value of arg.
arg = ABC
=== Call function ===
arg = ABC
# After change the value of arg.
arg = abc
-- Configuring done
-- Generating done
-- Build files have been written to: /home/yngzmiao/test/build
```

从这里可以看出，**宏实现的仅仅是字符串替换，宏定义的过程中是无法进行修改的，而函数却是可以的**。

### 蛋疼的参数
一般情况下，从上面的例子就能看出宏和函数的用法了，但很多情况下，我们自以为的“懂了”都是假懂。比如一不小心，就会出错。

更换`test.cmake`为下面的内容，并运行：
```cmake
set(var "ABC")

macro(Moo arg)
  message("arg = ${arg}")
  set(arg "abc")
  message("# After change the value of arg.")
  message("arg = ${arg}")
endmacro()
message("=== Call macro ===")
Moo(var)

function(Foo arg)
  message("arg = ${arg}")
  set(arg "abc")
  message("# After change the value of arg.")
  message("arg = ${arg}")
endfunction()
message("=== Call function ===")
Foo(var)
```

运行后的输出结果是：
```bash
-- The C compiler identification is GNU 5.4.0
-- The CXX compiler identification is GNU 5.4.0
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
=== Call macro ===
arg = var
# After change the value of arg.
arg = var
=== Call function ===
arg = var
# After change the value of arg.
arg = abc
-- Configuring done
-- Generating done
-- Build files have been written to: /home/yngzmiao/test/build
```

对比两段程序可以看出其中的区别：**无论是宏还是函数，当调用的时候如果使用的是`set`出来的变量，都必须通过`${}`将变量的内容传递进去，而不能只写上变量名**。

这是将实参传递给形参时的注意点，但在宏和函数的实现过程中，还有需要注意的内容。

例子：
```cmake
set(var "ABC")

macro(Moo arg)
  if (arg STREQUAL "ABC")
    message("arg1 = ${arg}")
  endif()
  if (${arg} STREQUAL "ABC")
    message("arg2 = ${arg}")
  endif()
endmacro()
message("=== Call macro ===")
Moo(${var})

function(Foo arg)
  if (arg STREQUAL "ABC")
    message("arg1 = ${arg}")
  endif()
  if (${arg} STREQUAL "ABC")
    message("arg2 = ${arg}")
  endif()
endfunction()
message("=== Call function ===")
Foo(${var})
```

运行后的输出结果是：
```bash
-- The C compiler identification is GNU 5.4.0
-- The CXX compiler identification is GNU 5.4.0
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
=== Call macro ===
arg2 = ABC
=== Call function ===
arg1 = ABC
arg2 = ABC
-- Configuring done
-- Generating done
-- Build files have been written to: /home/yngzmiao/test/build
```

可以看出，在宏和函数的实现过程中，**宏的参数由于不是传统意义上的变量，而是字符串替换，因此需要通过`${}`取出内容。而函数却不一定需要这样**。

也就是说，对于`macro`宏而言：
```cmake
if(argv0)                         # 错误用法
if(${argv0})                      # 正确用法
if(defined argv0)                 # 错误用法
if(defined ${argv0})              # 正确用法
```

也就是说，对于宏和函数的参数而言：
* **当宏和函数调用的时候，如果传递的是经`set`设置的变量，必须通过`${}`取出内容**；
* **在宏的定义过程中，对变量进行的操作必须通过`${}`取出内容，而函数就没有这个必要**。

### 相关阅读
[CMake实战--include命令和macro宏](https://www.jianshu.com/p/c2f4d95a1f4e)
[cmake使用教程（八）-macro和function](https://blog.csdn.net/weixin_34121282/article/details/87972772)
[CMake中include指令用法介绍](https://blog.csdn.net/liitdar/article/details/81144461)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">
