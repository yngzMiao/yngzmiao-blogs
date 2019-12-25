
很多时候，对于C++软件工程师来说，可能**需要编译能够在Android版本上直接运行的可执行程序、或者是编译so库文件**，这个时候就需要完成交叉编译。在命令行下执行交叉编译有两种方式：
* 一是用**NDK自带的工具链**
* 二是使用**独立工具链**

一般来说，只要使用NDK自带的工具链即可满足日常的需求。本文就这种交叉编译方式进行讲解。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191219](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191219)。

<br/>

## 前期准备
对于C++软件工程师而言，如果需要进行交叉编译，一般需要准备如下内容：
* 下载好NDK、CMake
* 已完成的C/C++文件
* 已完成的CMakeList.txt文件

准备好这些就可以进行Android的交叉编译了。

如果对CMakeList.txt不太熟悉，可以参考博文：[【CMake】CMakeLists.txt的超傻瓜手把手教程（附实例源码）](https://blog.csdn.net/qq_38410730/article/details/102477162)。

其实交叉编译的步骤，和一般情况下CMake的编译的步骤很类似：
```bash
mkdir build && cd build
cmake ..
make
make install
```

两者之间的不同完全在于：**编译工具链的不同**。而这个不同，体现在`cmake ..`的时候需要添加`-D`的编译变量和参数。

<br/>

## 编译变量与参数
官方文档地址：[CMake | Android NDK | Android Developers](https://developer.android.com/ndk/guides/cmake?hl=zh-cn)。

下表介绍在将CMake和NDK搭配使用时，可以配置的部分变量：
| 编译参数 | 说明 |
| :---: | :---: |
| **ANDROID_PLATFORM** | **指定目标Android平台的名称，如android-18指定Android 4.3(API级别18)** |
| ANDROID_STL | 指定CMake应使用的STL，默认c++_static |
| ANDROID_PIE | 指定是否使用位置独立的可执行文件(PIE)。Android动态链接器在Android 4.1(API级别16)及更高级别上支持PIE，可设置为On、OFF |
| ANDROID_CPP_FEATURES | 指定CMake编译原生库时需使用的特定C++功能，可设置为rtti(运行时类型信息)、exceptions(C++异常) |
| ANDROID_ALLOW_UNDEFINED_SYMBOLS | 指定CMake在构建原生库时，如果遇到未定义的引用，是否会引发未定义的符号错误。默认FALSE |
| ANDROID_ARM_NEON | 指定CMake是否应构建支持NEON的原生库。API级别为23或更高级别时，默认值为true，否则为false |
| ANDROID_DISABLE_FORMAT_STRING_CHECKS | 指定是否在编译源代码时保护格式字符串。启用保护后，如果在printf样式函数中使用非常量格式字符串，则编译器会引发错误。默认false |

下表介绍在Android进行交叉编译时，可以使用的具体构建参数，将有助于调试CMake构建问题：
| 编译参数 | 说明 |
| :---: | :---: |
| **ANDROID_ABI** | **目标ABI，可设置为armeabi-v7a、arm64-v8a、x86、x86_64，默认armeabi** |
| **ANDROID_NDK** | **安装的NDK根目录的绝对路径** |
| **CMAKE_TOOLCHAIN_FILE** | **进行交叉编译的android.toolchain.cmake文件的路径，默认在$NDK/build/cmake/目录** |
| ANDROID_TOOLCHAIN | CMake使用的编译器工具链，默认为clang |
| CMAKE_BUILD_TYPE | 配置构建类型，可设置为Release、Debug |
| ANDROID_NATIVE_API_LEVEL | CMake进行编译的Android API级别 |
| CMAKE_LIBRARY_OUTPUT_DIRECTORY | 构建LIBRARY目标文件之后，CMake存放这些文件的位置 |

看着编译的变量和参数都不少，那如何来抉择呢？

其实，一般情况下，只需要配置`ANDROID_ABI`、`ANDROID_NDK`、`CMAKE_TOOLCHAIN_FILE`、`ANDROID_PLATFORM`四个变量即可。

**ANDROID_ABI是CPU架构，ANDROID_NDK是NDK的根目录，CMAKE_TOOLCHAIN_FILE是工具链文件，ANDROID_PLATFORM是支持的最低Android平台**。

为什么指定ANDROID_PLATFORM？如果不指定，Android平台版本较低，此时ANDROID_PIE默认为OFF，可执行程序无法执行。

<br/>

## 实例
实例内容：编译C++程序为可执行文件，程序内容实现打印语句Hello World。

主程序内容特别简单：
```c
# include <iostream>

int main(int argc, char const *argv[])
{
  for(int i = 0; i < 5; ++i)
    std::cout << "Hello World" << std::endl;

  return 0;
}
```

编写CMakeLists.txt：
```
cmake_minimum_required(VERSION 3.0)
project(main)

add_definitions("-Wall -g")

add_executable(${PROJECT_NAME} main.cpp )

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR})
```

编写编译脚本：
```bash
#/bin/bash

export ANDROID_NDK=/opt/env/android-ndk-r14b

rm -r build
mkdir build && cd build 

cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
	-DANDROID_ABI="armeabi-v7a" \
	-DANDROID_NDK=$ANDROID_NDK \
	-DANDROID_PLATFORM=android-22 \
	..

make && make install

cd ..
```
此时会生成可执行文件main，push到Android设备上运行，可以看到Hello World的打印信息。

<br/>

## 相关阅读
* [cmake使用教程（九）-关于安卓的交叉编译](https://juejin.im/post/5a8ebe006fb9a0635a6574de)
* [在命令行下用cmake交叉编译可在android中运行的so包](https://blog.csdn.net/MingHuang2017/article/details/78938852)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">