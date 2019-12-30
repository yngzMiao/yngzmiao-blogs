
很多时候，写代码是一件很爽的事情，但最后需要对APP进行瘦身、性能分析却是一件很棘手的事情。当需要对APP的性能进行分析时，`Simpleperf`是一个简单快捷的选择。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191210](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191210)。

<br/>

正文开始前，先奉上官方的资料：
* [Simpleperf 命令和选项参考](https://developer.android.google.cn/ndk/guides/simpleperf-commands)
* [Simpleperf 用法](https://developer.android.google.cn/ndk/guides/simpleperf)

<br/>

## 什么是Simpleperf
`Simpleperf`是NDK软件包中提供的一种多功能命令行工具，方便为`Android`应用流程执行`CPU`分析。该工具可帮助找到使用Java、C/C++和Kotlin编写的应用的热点(所谓热点，也就是占用应用大部分执行时间的部分原生代码)。

也就是说，Simpleperf是NDK自带的工具，官方认证的CPU分析工具。


**Simpleperf是Android平台的一个本地层性能分析工具。它的命令行界面支持与linux-tools perf大致相同的选项**，但是它还支持许多Android特有的改进。

<br/>

Simpleperf的获取路径：[Google Git Simpleperf](https://android.googlesource.com/platform/prebuilts/simpleperf/)。

在页面内，可以直接压缩包下载，既可以选择NDK相对应的版本(R13~N21)，也可以选择master。建议直接选择NDK对应的版本即可。当然，也可以`git`直接拉取仓库。

```bash
git clone https://android.googlesource.com/platform/prebuilts/simpleperf
```

查看下载的simpleperf目录，可以看出：**它的工具集包涵client端和host端；client端运行在Android系统上，负责收集性能数据；host端则运行在开发机上，负责对数据进行分析和可视化(这些可执行文件在下载后的bin文件夹的android和win/linux下)**。

除了bin文件夹之外，最上层还有很多`.py`的脚本文件和`.config`的配置文件。**这些脚本和配置文件主要是官方写的一些傻瓜式的使用脚本，只需要对配置文件进行配置，就可以在直接在开发机上直接运行脚本，一键生成最终的结果**。

主要的脚本是：`app_profiler.py`和`report.py`两个。

脚本的主要内容，就是读取配置文件，然后执行`adb shell ...`的命令，其实本质上和命令行的输入没什么区别。但是，如果直接运行，不仅仅需要查看配置文件各个配置项的含义，还可能会出现许多意想不到的BUG，不太建议直接使用脚本，不得精髓啊。

<br/>

## Simpleperf的工作原理
现代的CPU具有一个硬件组件，称为性能监控单元(`PMU`)。**PMU具有一些硬件计数器，计数一些诸如经历了多少次CPU周期，执行了多少条指令，或发生了多少次缓存未命中等的事件。Linux内核将这些硬件计数器包装到硬件perf事件(hardware perf events)中。此外，Linux内核还提供了独立于硬件的软件事件和跟踪点事件。Linux内核通过perf_event_open系统调用将这些都暴露给了用户空间**。

这正是simpleperf所使用的机制。

Simpleperf具有三个主要的功能：**stat、record和report**。

`Stat`命令给出了**在一个时间段内被分析的进程中发生了多少事件的摘要**。以下是它的工作原理：
* 给定用户选项，simpleperf通过对linux内核进行系统调用来启用分析；
* Linux 内核在调度到被分析进程时启用计数器；
* 分析之后，simpleperf从内核读取计数器，并报告计数器摘要。

`Record`命令**在一段时间内记录剖析进程的样本**。它的工作原理如下：
* 给定用户选项，simpleperf通过对linux内核进行系统调用来启用分析；
* Simpleperf在simpleperf和linux内核之间创建映射缓冲区；
* Linux内核在调度到被分析进程时启用计数器；
* 每次给定数量的事件发生时，linux内核将样本转储到映射缓冲区；
* Simpleperf从映射缓冲区读取样本并生成perf.data。

`Report`命令**读取perf.data文件及所有被剖析进程用到的共享库，并输出一份报告，展示时间消耗在了哪里**。

<br/>

## Simpleperf的使用
尽管不推荐使用一键脚本，但得益于Simpleperf的命令不多，使用起来也很简单快捷。

### 文件准备
将simpleperf可执行文件传输到Android设备上:
```bash
adb push 文件源地址 文件目的地址
```
可执行文件都存储在`bin/android/`目录下，可自行获取。

### 性能分析
在性能分析前，首先启动需要分析的APP，获取APP的进程号：
```bash
adb shell am start -n 包名/启动程序
adb shell pidof 包名
```
接下来，就可以使用命令三连击了：
```bash
adb shell
cd simpleperf所在目录
./simpleperf stat [options] [command [command-args]]
./simpleperf record [options] [command [command-args]]
./simpleperf report [options]
```
stat命令获取一段时间内已分析进程中发生的事件数摘要。 最常使用的选项为：
```bash
./simpleperf stat -p 进程号 --duration 检测进程的持续时间(秒)
```
record命令记录一段时间内已分析进程的样本，这是simpleperf的主命令。最常使用的选项为：
```bash
./simpleperf record -p 进程号 -o 输出文件(默认perf.data) --duration 监测进程的持续时间(秒)
```
需要注意的是，如果出现`Access to kernel symbol addresses is restricted`的警告，需要使用一下命令来取消：
```bash
echo 0>/proc/sys/kernel/kptr_restrict
```
report命令读取perf.data文件(由`simpleperf record`创建)，并显示报告，表明花费时间的地方。最常使用的选项为：
```bash
./simpleperf report --dsos 选定动态共享对象(so库)  -f 记录文件(默认perf.data) --sort 用于排序和打印报告的键 -n
```

### 注意点
**如果使用report命令进行查找的时候，发现so现实的Symbol都是地址，而不是函数内容。这多数是因为在安卓编译的时候，设备上使用的so库已经被strip过，也就是说，已经抛离了.symbol段的内容**。

那么，我们需要将带有`Symbol`信息的so下载到设备上。同时**需要将so放置到perf.data中记录的相同的路径(否则，simpleperf无法找到它)**。

如果找不到路径，可以在`perf.data`文件中直接搜索需要选定的so库的名称，即可查看到路径。

<br/>

## 实例
设备上执行命令：
```bash
./simpleperf  stat -p 5932 --duration 10
./simpleperf record -p 5932 -o dms.data --duration 10
./simpleperf report --dsos /data/app/com.hobot.dms.sample-1/lib/arm/libhobot_dms.so  --sort comm,pid,tid,symbol -n
```
通过report命令可以看到，主要是集中在libhobot_dms.so中。但是，显示的都是地址，并非函数调用。这是因为Android的so库已经抛离了.symbol段的内容。可以重新指定完整的so文件。
<center><img src="https://img-blog.csdnimg.cn/20191211155836950.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70" width="85%">

将perf.data从设备上拷贝下来，传输到Linux开发机上，执行命令：
```bash
./bin/linux/x86_64/simpleperf report --dsos /data/app/com.hobot.dms.sample-1/lib/arm/libhobot_dms.so  --sort comm,pid,tid,symbol
```
<center><img src="https://img-blog.csdnimg.cn/20191211155507947.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70" width="85%">

可以看到CvtFormat函数是最耗时的函数，需要被优化。
<br/>

## 相关阅读
* [另一个Android性能剖析工具——simpleperf](https://zhuanlan.zhihu.com/p/25277481)
* [Simpleperf介绍](https://www.jianshu.com/p/90f4afbdd2ae)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">