有些时候，我们需要在项目中**标明版本号、Git的hash号、编译时间**等信息，但是显然，对于Git的hash号、编译时间我们不想自己手动填写。现在提供一种途径，将这些信息写入到头文件中，再编译到so库文件或者可执行程序中。

这样，就可以通过提供库文件的接口或者可执行程序的打印中得到这些值了。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q1/20200106](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q1/20200106)。

<br/>

## configure_file
```
configure_file(<input> <output>
               [COPYONLY] [ESCAPE_QUOTES] [@ONLY]
               [NEWLINE_STYLE [UNIX|DOS|WIN32|LF|CRLF] ])
```
官方CMake教程对它的解释是：将文件复制到另一个位置并修改其内容。

当然，这里的修改其内容也不是任意地修改，也是遵循一定的规则：**将input文件复制到output文件，并在输入文件内容中的变量，替换引用为@VAR@或${VAR}的变量值。每个变量引用将替换为该变量的当前值，如果未定义该变量，则为空字符串**。

可能有些绕头，再浅显一点：**configure_file，复制一份输入文件到输出文件，替换输入文件中被@VAR@或者${VAR}引用的变量值。也就是说，让普通文件，也能使用CMake中的变量**。例如：

比如在CMakeLists.txt中定义了如下的变量：
```
set(BUILD_Version 1)
```
输入文件中为：
```
#define BUILD_Version @BUILD_Version@
```
那么，在输出文件中就会被转化为：
```
#define BUILD_Version 1
```

顺便，看一下configure_file的其他选项：
* **COPYONLY**：只拷贝文件，不进行任何的变量替换。这个选项在指定了`NEWLINE_STYLE`选项时不能使用（无效）。
* **ESCAPE_QUOTES**：躲过任何的反斜杠(C风格)转义。
* **@ONLY**：限制变量替换，让其只替换被`@VAR@`引用的变量(那么`${VAR}`格式的变量将不会被替换)。这在配置`${VAR}`语法的脚本时是非常有用的。
* **NEWLINE_STYLE style**：指定输出文件中的新行格式。UNIX和LF的新行是\n，DOS和WIN32和CRLF的新行格式是\r\n。这个选项在指定了COPYONLY选项时不能使用(无效)。

通常情况下，**输入文件以.h.in为后缀，输出文件以.h为后缀**。

<br/>

## 实例
实例内容：在CMakeLists.txt中指定版本号、自动获取Git的分支和hash号、自动获取编译时间，并将这些信息写入到可执行程序中打印出来。

### 前提
需要读懂下面的程序内容，需要掌握知识点：
* [【CMake】cmake中的include指令（.cmake文件/MACRO宏/function函数）](https://blog.csdn.net/qq_38410730/article/details/102677143)
* [【CMake】CMakeLists.txt的超傻瓜手把手教程（附实例源码）](https://blog.csdn.net/qq_38410730/article/details/102477162)
* [【CMake】cmake的install指令](https://blog.csdn.net/qq_38410730/article/details/102837401)

### 程序内容
编写`utils.cmake`，定义`macro`宏，用于获取Git的hash值和分支：
```cmake
# get git hash
macro(get_git_hash _git_hash)   # 宏的开始
    find_package(Git QUIET)     # 查找Git，QUIET静默方式不报错
    if(GIT_FOUND)
      execute_process(          # 执行一个子进程
        COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%h # 命令
        OUTPUT_VARIABLE ${_git_hash}        # 输出字符串存入变量
        OUTPUT_STRIP_TRAILING_WHITESPACE    # 删除字符串尾的换行符
        ERROR_QUIET                         # 对执行错误静默
        WORKING_DIRECTORY                   # 执行路径
          ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
endmacro()                      # 宏的结束

# get git branch
macro(get_git_branch _git_branch)   # 宏的开始
    find_package(Git QUIET)     # 查找Git，QUIET静默方式不报错
    if(GIT_FOUND)
      execute_process(          # 执行一个子进程
        COMMAND ${GIT_EXECUTABLE} symbolic-ref --short -q HEAD
        OUTPUT_VARIABLE ${_git_branch}        # 输出字符串存入变量
        OUTPUT_STRIP_TRAILING_WHITESPACE    # 删除字符串尾的换行符
        ERROR_QUIET                         # 对执行错误静默
        WORKING_DIRECTORY                   # 执行路径
          ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
endmacro()                      # 宏的结束
```
编写CMakeLists.txt，定义编译时间戳变量、版本变量、Git信息变量，同时实现编译可执行程序、复制文件和变换变量：
```cmake
cmake_minimum_required(VERSION 3.0)
include(cmake/utils.cmake)
project(main)

string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")
message("Build timestamp is ${BUILD_TIMESTAMP}")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)
message("Version is ${VERSION_MAJOR} ${VERSION_MINOR} ${VERSION_PATCH}")

set(GIT_HASH "")
get_git_hash(GIT_HASH)
message("Git hash is ${GIT_HASH}")

set(GIT_BRANCH "")
get_git_branch(GIT_BRANCH)
message("Git branch is ${GIT_BRANCH}")

configure_file (
    "${PROJECT_SOURCE_DIR}/include/utils.h.in"
    "${PROJECT_SOURCE_DIR}/include/utils.h"
)

add_executable(${PROJECT_NAME} src/main.cc)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR})
```
configure_file的输入文件：
```c
#ifndef UTILS_H_IN
#define UTILS_H_IN

#define VERSION_MAJOR @VERSION_MAJOR@
#define VERSION_MINOR @VERSION_MINOR@
#define VERSION_PATCH @VERSION_PATCH@

#define BUILD_TIMESTAMP "@BUILD_TIMESTAMP@"

#define GIT_BRANCH "@GIT_BRANCH@"
#define GIT_HASH "@GIT_HASH@"

#endif // UTILS_H_IN
```
主程序为：
```c
#include "utils.h"
#include <iostream>

int main(int argc, char const *argv[])
{
  std::cout << "version is " << VERSION_MAJOR << ", " << VERSION_MINOR << ", "<< VERSION_PATCH << std::endl;
  std::cout << "timestamp is " << BUILD_TIMESTAMP << std::endl;
  std::cout << "git is " << GIT_BRANCH << ", " << GIT_HASH << std::endl;
  
  return 0;
}
```

### 编译运行
编译并运行该项目，会生成新的文件：
```c
#ifndef UTILS_H_IN
#define UTILS_H_IN

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 1

#define BUILD_TIMESTAMP "2019-12-27 20:28:57"

#define GIT_BRANCH "master"
#define GIT_HASH "14e77d2"

#endif // UTILS_H_IN
```
运行结果为：
```bash
yngzmiao@yngzmiao-virtual-machine:~/test/test$ ./main 
version is 0, 0, 1
timestamp is 2019-12-27 21:28:57
git is master, 14e77d2
```

<br/>

## 相关阅读
* [奇技淫巧[1]：cmake中获取git信息](https://zhuanlan.zhihu.com/p/57319492)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">
