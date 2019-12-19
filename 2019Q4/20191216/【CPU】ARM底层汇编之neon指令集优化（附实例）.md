当在`ARM`芯片上进行一些例如图像处理等计算的时候，常常会因为计算量太大造成计算帧率较低的情况。因而，需要选择一种更加简单快捷的计算方式以获得处理速度上的提升。`ARM NEON`就是一个不错的选择。

<br/>

## Neon指令优化
NEON是一种`SIMD`(`Single Instruction Multiple Data`)指令，也就是说，NEON**可以把若干源(source)操作数(operand)打包放到一个源寄存器中，对他们执行相同的操作，产生若干目的(dest)操作数，这种方式也叫向量化(vectorization)**。

可能你对这个描述还不够清晰，简单来说，就是：**NEON指令优化的精髓就在于同时在不同通道内进行并行运算。通常可用于图像等矩阵数据的循环优化**。

更简单的说，就是，将Neon寄存器分为多个通道，每个通道存储一个数据。一条对Neon寄存器的计算指令，实际上，是对各通道的数据分别的计算指令。即寄存器位宽，直接影响到数据的通道数。

例如：在ARMv7的NEON unit中，`register file`总大小是1024-bit，可以划分为16个128-bit的`Q-register`(Quadword register)或者32个64-bit的`D-register`(Dualword register)，也就是说，最长的寄存器位宽是128-bit。那么，假设我们采用32-bit单精度浮点数float来做浮点运算，那么可以把最多128/32=4个浮点数打包放到Q-register中做运算，即4个4个参与计算，从而提高吞吐量，减少loop次数。

<br/>

## Neon指令的使用
主流支持目标平台为ARM CPU的编译器基本都支持NEON指令。**可以通过在代码中嵌入NEON汇编来使用NEON，但是更加常见的方式是通过类似C函数的NEON Instrinsic来编写NEON代码**。本文统一采用后者。
### 硬件平台
本文的例子都是基于`ARMV7`架构平台。ARMV7架构包含：
* 16个通用寄存器(32bit)，R0-R15
* 16个NEON寄存器(128bit)，Q0-Q15(同时也可以被视为32个64bit的寄存器，D0-D31)
* 16个VFP寄存器(32bit)，S0-S15
其中：**NEON和VFP的区别在于VFP是加速浮点计算的硬件不具备数据并行能力，同时VFP更尽兴双精度浮点数（double）的计算，NEON只有单精度浮点计算能力**。
### 头文件和编译选项
在使用`NEON Instrinsic`来进行编写NEON代码前，需要引入头文件：
```c
#include <arm_neon.h>
```
同时，在编译的时候，需要指定编译参数。如果使用CMakeLists.txt，可以指定：
```bash
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon")
```
关于编译选项，可以参考：[ARM平台NEON指令的编译和优化](https://blog.csdn.net/heli200482128/article/details/79303286)
### NEON Instrinsic详细解释
* [ARM NEON Intrinsics](https://gcc.gnu.org/onlinedocs/gcc-4.7.4/gcc/ARM-NEON-Intrinsics.html#ARM-NEON-Intrinsics)
* [ARM汇编笔记（3）——NEON intrinsics 函数](https://blog.csdn.net/sunty2016/article/details/79857825)
#### 数据类型
对于数据类型的命名，一般遵循这样的规则：
```
<基本类型>x<lane个数>x<向量个数>_t
```
其中，向量个数如果省略表示只有一个。

基本类型int8，int16，int32，int64，uint8，uint16，uint32，uint64，float16，float32

lane个数表示并行处理的基本类型数据的个数。

按照上述的规则，比如：
```
float32x4_t 
```

#### 指令函数
对于指令函数的命名，一般遵循这样的规则：
```
v<指令名>[后缀]_<数据基本类型简写>
```
其中，**后缀如果没有，表示64位并行；如果后缀是q，表示128位并行；如果后缀是l，表示长指令，输出数据的基本类型位数是输入的2倍；如果后缀是n，表示窄指令，输出数据的基本类型位数是输入的一半**。

数据基本类型简写：s8，s16，s32，s64，u8，u16，u32，u64，f16，f32。

按照上述的规则，比如：
```c
vadd_u16：两个uint16x4相加为一个uint16x4
vaddq_u16：两个uint16x8相加为一个uint16x8
vaddl_u16：两个uint8x8相加为一个uint16x8
```
#### 指令名
Neon的指令名主要分为：**算术和位运算指令、数据移动指令、访存指令**。
算术和位运算指令最简单，包括add(加法)，sub(减法)，mul(乘法)这些基本指令。

实际编程中经常要在不同NEON数据类型间转移数据，有时还要按lane来get/set向量值，NEON intrinsics也提供了这类操作。
* dup[后缀]_n_<数据基本类型简写>：用同一个标量值初始化一个向量全部的lane；
* set[后缀]_lane_<数据基本类型简写>：对指定的一个lane进行设置
* get[后缀]_lane_<数据基本类型简写>：获取指定的一个lane的值
* mov[后缀]_<数据基本类型简写>：数据间移动

NEON访存指令可以将内存读到NEON数据类型中去，或者将NEON数据类型写进内存。可以支持一次读写多向量数据类型。
* ld<向量数>[后缀]_<数据基本类型简写>：读内存
* st<向量数>[后缀]_<数据基本类型简写>：写内存

<br/>

## 实例
实例内容：对于1280 * 720 * 3的图片数据，需要对每个像素点进行同样的加法和乘法运算，比较非Neon和Neon两种方式的耗时。
源码：
```c
# include <iostream>
# include <chrono>
# include <random>
#include <arm_neon.h>

int main(int argc, char const *argv[])
{
  float *data_tmp = new float[1080 * 720 * 3];
  std::default_random_engine e;
  std::uniform_real_distribution<float> u(0, 255);
  for(int i = 0; i < 1080 * 720 * 3; ++i) {
    *(data_tmp + i) = u(e);
  }

  float *data = data_tmp;
  float *data_res1 = new float[1080 * 720 * 3];

  std::chrono::microseconds start_time = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  );

  for(int i = 0; i < 1080 * 720 * 3; ++i) {
    *data_res1 = ((*data) + 3.4 ) / 3.1;
    ++data_res1;
    ++data;
  }

  std::chrono::microseconds end_time = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  );

  std::cout << "cost total time : " << (end_time - start_time).count() << " microseconds  -- common method" << std::endl;

  data = data_tmp;
  float *data_res2 = new float[1080 * 720 * 3];

  start_time = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  );

  float32x4_t A = vdupq_n_f32(3.4);
  float32x4_t B = vdupq_n_f32(3.1);
  for(int i = 0; i < 1080 * 720 * 3 / 4; ++i) {
    float32x4_t C = (float32x4_t){*data, *(data + 1), *(data + 2), *(data + 3)};
    float32x4_t D = vmulq_f32(vaddq_f32(C, A), B);
    vst1q_f32(data_res2, D);
    data = data + 4;
    data_res2 = data_res2 + 4;
  }

  end_time = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  );

  std::cout << "cost total time : " << (end_time - start_time).count() << " microseconds  -- neon method" << std::endl;

  return 0;
}
```
编写CMakeLists.txt，用于项目编译：
```bash
cmake_minimum_required(VERSION 3.0)
project(main)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon")
add_definitions("-Wall -g")

add_executable(${PROJECT_NAME} main.cpp )

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${PROJECT_SOURCE_DIR})
```
在同级目录下编写main.sh，进行项目编译：
```bash
#/bin/bash

export ANDROID_NDK=/opt/env/android-ndk-r14b

rm -r build
mkdir build && cd build 

cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
	-DANDROID_ABI="armeabi-v7a" \
	-DANDROID_PLATFORM=android-22 \
	..

make && make install

cd ..
```
将生成的可执行文件main，push到设备端进行运行，最终的运行结果：
```
cost total time : 112538 microseconds  -- common method
cost total time : 44217 microseconds  -- neon method
```
可以看出，使用Neon指令集优化，省下了近60.71%的运行时间。

<br/>

## 相关阅读
* [ARM底层汇编优化之NEON优化 - 概述（基础入门 ）](https://zhuanlan.zhihu.com/p/61356656)
* [ARM NEON编程初探——一个简单的BGR888转YUV444实例详解](https://segmentfault.com/a/1190000010127521)
* [ARM NEON 编程系列2 - 基本指令集](https://www.cnblogs.com/xylc/p/5410517.html)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">