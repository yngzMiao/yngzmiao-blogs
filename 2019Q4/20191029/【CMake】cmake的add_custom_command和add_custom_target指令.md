在很多时候，需要在`cmake`中创建一些目标，如`clean`、`copy`等等，这就需要通过`add_custom_target`来指定。同时，`add_custom_command`可以用来完成对`add_custom_target`生成的`target`的补充。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191029](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191029)。

<br/>

## add_custom_target
如果你写过`MakeFile`，那么一定知道，可以设定很多的目标来`make`，如：
```cmake
target ... : prerequisites ...  
    command
```
其中：
* **target是下面的命令的目标**，即下面命令是为了`target`而生的。这个目标可以是`*.o`文件，也可以是可执行文件；
* **prerequisites则是生成该目标所依赖的文件**，如果找不到依赖的文件，下面的命令就不会执行且会中断`make`；
* **command就是生成目标文件的命令**，一般就是编译命令。

那么，如果使用`CMakeLists.txt`如何也生成一个目标来`make`呢？

这就是`add_custom_target`的用处：**增加一个没有输出的目标，使得它总是被构建**。

```cmake
add_custom_target(Name [ALL] [command1 [args1...]]
                  [COMMAND command2 [args2...] ...]
                  [DEPENDS depend depend depend ... ]
                  [BYPRODUCTS [files...]]
                  [WORKING_DIRECTORY dir]
                  [COMMENT comment]
                  [JOB_POOL job_pool]
                  [VERBATIM] [USES_TERMINAL]
                  [COMMAND_EXPAND_LISTS]
                  [SOURCES src1 [src2...]])
```
乍一看，该命令有很多的参数，但其实我们并不需要全部了解，甚至一般情况下只需要用到其中的两三个。

例如：
```cmake
cmake_minimum_required(VERSION 3.0)
project(test)

add_custom_target(CopyTask
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/config ${CMAKE_CURRENT_SOURCE_DIR}/etc
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/log.txt ${CMAKE_CURRENT_SOURCE_DIR}/etc
  )
```
运行该CMakeLists.txt：
```bash
mkdir build && cd build
cmake ..
make CopyTask
```

运行结果为：
```bash
yngzmiao@yngzmiao-virtual-machine:~/test/build$ rm -r *;cmake ..
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
-- Configuring done
-- Generating done
-- Build files have been written to: /home/yngzmiao/test/build
yngzmiao@yngzmiao-virtual-machine:~/test/build$ make CopyTask
Scanning dependencies of target CopyTask
Built target CopyTask
```

命令行的打印信息看不出什么东西，但是如果根目录下有`config`文件夹，或者有`log.txt`文件，那么文件夹内的文件和`log.txt`会被拷贝到`etc`文件夹下。

初始目录结构，如下：
```bash
|---build
|---config
        |---log.log
|---CMakeLists.txt
|---log.txt
```
运行后目录结构，如下：
```bash
|---build
|---config
        |---log.log
|---etc
        |---log.log
        |---log.txt
|---CMakeLists.txt
|---log.txt
```

其实可以看出，这段代码的目的就是将`config`文件夹的内容和`log.txt`文件复制到新的`etc`文件夹内。

`add_custom_target`生成一个目标`CopyTask`，该目标是用来复制文件夹或者复制文件的！也就是`COMMAND`中定义的操作。

其中：**${CMAKE_COMMAND}是CMake的路径，-E使CMake运行命令而不是构建，copy_directory和copy是cmake_command_line，再后面两个就是command_line的参数**。

当然，生成文件不仅仅只能是复制，还可以是其他的操作。而**这些COMMAND操作，都在command_line中规定了**。

至于`cmake_command_line`的内容，可参考`cmake`的官方资料：[Run a Command-Line Tool](https://cmake.org/cmake/help/latest/manual/cmake.1.html#run-a-command-line-tool)。

该命令的其他一些参数的含义：
* ALL：表明该目标会被添加到默认的构建目标，使得它每次都被运行；
* COMMAND：指定要在构建时执行的命令行；
* DEPENDS：指定命令所依赖的文件；
* COMMENT：在构建时执行命令之前显示给定消息；
* WORKING_DIRECTORY：使用给定的当前工作目录执行命令。如果它是相对路径，它将相对于对应于当前源目录的构建树目录；
* BYPRODUCTS：指定命令预期产生的文件。

<br/>

## add_custom_command
按照官方资料翻译的话，这个命令的用途是：**将自定义构建规则添加到生成的构建系统**。

什么鬼！我猜，你一定和我一样蒙，完全不太能够理解是什么意思。既然无法理解它的语言版本的解释，那就在具体的例子中进行熟悉。

该命令主要用于两种场景下：
### 生成文件
这种场景是：**添加定制命令来生成文件**。它的命令内容为：

```cmake
add_custom_command(OUTPUT output1 [output2 ...]
                   COMMAND command1 [ARGS] [args1...]``
                   [COMMAND command2 [ARGS] [args2...] ...]
                   [MAIN_DEPENDENCY depend]
                   [DEPENDS [depends...]]
                   [BYPRODUCTS [files...]]
                   [IMPLICIT_DEPENDS <lang1> depend1
                                    [<lang2> depend2] ...]
                   [WORKING_DIRECTORY dir]
                   [COMMENT comment]
                   [DEPFILE depfile]
                   [JOB_POOL job_pool]
                   [VERBATIM] [APPEND] [USES_TERMINAL]
                   [COMMAND_EXPAND_LISTS])
```

例如：
```cmake
cmake_minimum_required(VERSION 3.0)
project(test)

add_custom_command(OUTPUT COPY_RES
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/config ${CMAKE_CURRENT_SOURCE_DIR}/etc
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/log.txt ${CMAKE_CURRENT_SOURCE_DIR}/etc
  )

add_custom_target(CopyTask ALL DEPENDS COPY_RES)
```
`add_custom_target`生成一个目标`CopyTask`，该目标依赖于`COPY_RES`。而对于`COPY_RES`而言，它实际上是用来复制文件夹或者复制文件的！也就是`COMMAND`中定义的操作。

该命令的其他一些参数的含义：
* OUTPUT：指定命令预期产生的输出文件。如果输出文件的名称是相对路径，即相对于当前的构建的源目录路径；
* COMMAND：指定要在构建时执行的命令行；
* DEPENDS：指定命令所依赖的文件；
* COMMENT：在构建时执行命令之前显示给定消息；
* WORKING_DIRECTORY：使用给定的当前工作目录执行命令。如果它是相对路径，它将相对于对应于当前源目录的构建树目录；
* DEPFILE：为生成器指定一个.d depfile .d文件保存通常由自定义命令本身发出的依赖关系；
* MAIN_DEPENDENCY：指定命令的主要输入源文件；
* BYPRODUCTS：指定命令预期产生的文件。

### 构建事件
这种场景是：**为某个目标(如库或可执行程序)添加一个定制命令**。

**这种定制命令可以设置在，构建这个目标过程中的某些时机**。也就是就，这种场景可以在目标构建的过程中，添加一些额外执行的命令。这些命令本身将会成为该目标的一部分。注意，**仅在目标本身被构建过程才会执行。如果该目标已经构建，命令将不会执行**。

那么这些时机是什么呢？如下表所示：

| 参数 | 含义 |
| :---: | :---: |
| PRE_BUILD | 在目标中执行任何其他规则之前运行 |
| PRE_LINK | 在编译源代码之后，链接二进制文件或库文件之前运行 |
| POST_BUILD | 在目标内所有其他规则均已执行后运行 |

其中，`PRE_BUILD`只被`Visual Studio 7`及之后的版本支持，其他情况下`PRE_BUILD`都被视为`PRE_LINK`。

此时的命令为：
```cmake
add_custom_command(TARGET <target>
                   PRE_BUILD | PRE_LINK | POST_BUILD
                   COMMAND command1 [ARGS] [args1...]
                   [COMMAND command2 [ARGS] [args2...] ...]
                   [BYPRODUCTS [files...]]
                   [WORKING_DIRECTORY dir]
                   [COMMENT comment]
                   [VERBATIM] [USES_TERMINAL])
```

使用方式和第一种场景类似：
```cmake
cmake_minimum_required(VERSION 3.0)
project(test)

add_custom_target(CopyTask)

add_custom_command(TARGET CopyTask
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/config ${CMAKE_CURRENT_SOURCE_DIR}/etc
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/log.txt ${CMAKE_CURRENT_SOURCE_DIR}/etc
  )
```
这段代码的功能和上一段是一样的，将`config`文件夹的内容和`log.txt`文件复制到新的`etc`文件夹内。

需要注意的是，**此时add_custom_command需要写在add_custom_target之后，否则将cmake不通过**。

该命令的其他一些参数的含义：
* TARGET：指定命令运行的目标；
* COMMAND：指定要在构建时执行的命令行；
* COMMENT：在构建时执行命令之前显示给定消息；
* WORKING_DIRECTORY：使用给定的当前工作目录执行命令。如果它是相对路径，它将相对于对应于当前源目录的构建树目录；
* BYPRODUCTS：指定命令预期产生的文件。

### add_custom_command总结
其实，可以看出尽管官方给了两种的使用情景，但是本质上没有什么区别，区别在于：
* **如果使用OUTPUT参数，需要在目标的构建中指定依赖于该OUTPUT**；
* **如果使用TARGET参数，直接指定目标就可以了**。

<br/>

## add_custom总结
其实，可以发现，`add_custom_command`有时候略显鸡肋，因为我们可以将所有的`COMMAND`都写到`add_custom_target`中，不需要`add_custom_command`来进行补充。

### 相关阅读
[cmake整理：在编译时拷贝文件之add_custom_comand 和 add_custom_target](https://blog.csdn.net/gubenpeiyuan/article/details/51096777)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">