Protobuf是Google开源的一个项目，博主将会在几篇博文中对其进行讲解。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191225](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191225)。

<br/>

## 序列化和反序列化
有些时候，我们希望将对象保存到文件中，或者传输给其他的应用程序。比如：web网页后端和前端的数据交互，应用程序产生的中间数据等等。

这种**将数据结构或对象以某种格式转化为字节流的过程**，称之为`序列化`(Serialization)，目的是把当前的状态保存下来，在需要时复原数据结构或对象(序列化时不包含与对象相关联的函数，所以后面只提数据结构)。`反序列化`(Deserialization)，是序列化的逆过程，**读取字节流，根据约定的格式协议，将数据结构复原**。

在序列化和反序列化的过程中，需要注意的点：

* 代码运行过程中，数据结构和对象位于内存，其中的各项数据成员可能彼此紧邻，也可能分布在并不连续的各个内存区域，比如指针指向的内存块等。而文件中字节是顺序存储的，要想将数据结构保存成文件，就需要把所有的数据成员平`铺开`，然后串接在一起；
* 直接串接可能是不行的，因为字节流中没有天然的分界，所以在序列化时需要按照某种约定的格式(协议)，以便在反序列化时知道**从哪里到哪里是哪个数据成员**，因此格式可能需要约定：指代数据成员的标识、起始位置、终止位置、长度、分隔符等。

由上可见，`格式协议`是最重要的，它直接决定了序列化和反序列化的效率、字节流的大小和可读性等。

最常见的序列化和反序列化的格式就是json格式、XML格式，他们有自己的一套完整的格式协议。而本文讲解的格式是`Protobuf`。

<br/>

## Protobuf
先贴上Protobuf的官方资料：
* Protobuf官方资料：[Developer Guide](https://developers.google.com/protocol-buffers/docs/overview)
* Protobuf gitbub地址：[protocolbuffers/protobuf](https://github.com/protocolbuffers/protobuf)
### Protobuf简介
`Protobuf`是`Google`开发的一种用于序列化结构化数据(比如Java中的Object，C中的Structure)的语言中立、平台中立、可扩展的数据描述语言，可用于数据存储、通信协议等方面。Protocol Buffers可以理解为是更快、更简单、更小的JSON或者XML，区别在于**Protocol Buffers是二进制格式**，而JSON和XML是文本格式。

目前protobuf支持的语言包括：C++、C#、Java、JS、OC、PHP、Ruby这七种。

相比较于XML、json，Protobuf的优点：
* 简洁，**体积小**，消息大小只需要json的10分之一，XML的20分之一；
* **速度快**，解析速度比XML、json快20~100倍；
* 自动生成数据访问类，方便应用程序的使用。Protobuf编译器会将.proto文件编译生成对应的数据访问类；
* **向后兼容性好**，不必破坏旧数据格式的程序就可以对数据结构进行升级。不必担心因为消息结构的改变而造成的大规模的代码重构或者迁移的问题。

相对而言，Protobuf也有缺点：
* protobuf采用了二进制格式进行编码，可读性差；
* protobuf并非自描述的，必须要有格式定义文件(.proto 文件)。

既然Protobuf可以自动生成`数据访问类`，也就是说，只要规定了`.proto`文件，可以直接生成C++的.cc文件和.h文件，可以直接生成python的.py文件，可以直接生成Java的.java文件……

那么它是如何生成的呢？这就需要下载安装Protobuf了。

### Protobuf的下载安装
Protobuf的release版本，下载可以移步：<a href="https://github.com/protocolbuffers/protobuf/releases">Protobuf release版本</a>。

如果是Linux操作系统下，可以直接下载：<a href="https://github.com/protocolbuffers/protobuf/releases/download/v3.8.0/protoc-3.8.0-linux-x86_64.zip">protoc-3.8.0-linux-x86_64.zip</a>。

这个版本包含了protoc二进制文件以及与protobuf一起分发的一组标准.proto文件。 

进入bin文件夹，查看protoc的版本信息：
```c
./protoc --version
```
如果打印出了protoc的版本信息，就表示没有任何问题。

当然，你也可以选择下载Protobuf的源代码，然后通过解压、编译、安装来使用它。这种方式的下载路径为：<a href="https://github.com/protocolbuffers/protobuf/archive/v3.8.0.tar.gz">protobuf-3.8.0.tar.gz</a>。

python2安装步骤如下所示：
```c
tar -xzf protobuf-3.8.0.tar.gz 
cd protobuf-3.8.0 
./configure --prefix=$INSTALL_DIR 
make 
make check 
make install
```

python3安装步骤如下所示：
```c
tar -xzf protobuf-3.8.0.tar.gz 
cd protobuf-3.8.0 
./autogen.sh
./configure
make
make check
sudo make install
sudo ldconfig # refresh shared library cache.
```
很有可能，执行过程中会出现以下错误提示：
```bash
./autogen.sh: 4: ./autogen.sh: autoreconf: not found
```
解决办法：执行以下命令即可。
```bash
sudo apt-get install autoconf
sudo apt-get install automake
sudo apt-get install libtool
```
其实**推荐第一种安装方式**，在protobuf的使用过程中，一般只需要可执行文件即可。但是，如果**你使用C++版本，但自己没有Google对应的protobuf头文件和静态库，还是需要第二种安装方式**。

通常情况Protobuf都安装在`/usr/local`目录下，该目录下包含了Protobuf的**头文件，静态库和动态库文件**，如果是需要使用C++版本，头文件和静态库需要拷贝出来。

### Protobuf的使用
在使用Protobuf之前，需要提前创建一个.proto文件。在.proto文件中，需要定义要生成的数据访问类的成员信息等内容。然后，就可以指定该.proto文件来生成了。

如何直接生成呢？这就需要用到安装的可执行文件了。
```bash
./protoc 指定.proto文件 --cpp_out=./
./protoc 指定.proto文件 --java_out=./
./protoc 指定.proto文件 --python_out=./
```

<br/>

## .proto语法结构
每个定义结构化数据结构体的.proto文件，也需要按照一定的结构和语法进行编写，但这个语法是非常简单的。

提前声明一点，**proto2和proto3的语法规则是有一定的差异和不兼容性的，需要注意**。
### 版本声明
在编写.proto文件的最开始部分，需要指定.proto文件版本：
```proto
syntax = "proto2";                  //声明proto2版本(选其一)
syntax = "proto3";                  //声明proto3版本(选其一)
```

### 定义message结构
使用message定义一个消息类型，与C++、Java等高级语言对应起来就可以理解为`Class`。

每个message通常由**字段修饰符、字段类型、字段名、标识号**组成。

以Person为例，在`proto2`中：
```proto
message Person {
  required int32 id =1;
  required string name = 2;
  optional int32 age = 3;
  repeated string email = 4;
}
```
字段修饰符：只有三种字段修饰符(**required、optional、repeated**)，且每个字段必须有字段修饰符。
* required：表示该字段的是必须设置的（该限制体现在：若在对应语言中该字段处于未被赋值/初始化的状态，则会报错）；
* optional: 表示该字段的是可选设置的，可通过`[default=xxx]`指定一个默认值，若没有显示指定默认值并且该字段没有被设置，则会使用该类型的默认值；
* repeated: 表示该字段可以有多个值，一般会被编译为对应语言的集合类或数组。由于一些历史原因，基本数值类型的repeated的字段并没有被尽可能地高效编码。在新的代码中，用户应该使用特殊选项`[packed=true]`来保证更高效的编码。

字段类型：可以指定proto定义的数据类型，当然也可以指定自己定义的数据类型。proto定义的数据类型如下：
| .proto类型 | C++ Type | Java Type | Python Type | Note |
| :---: | :---: | :---: | :---: | :---: |
| double |  | double | double | float |
| float |  | float | float | float |
| int32 | 使用可变长度编码，**负数编码效率低下。如果可能具有负值，请改用sint32** | int32 | int | int |
| int64 | 使用可变长度编码，**负数编码效率低下。如果可能具有负值，请改用sint64** | int64 | long | int/long |
| uint32 | 使用可变长度编码 | uint32 | int | int/long |
| uint64 | 使用可变长度编码 | uint64 | long | int/long |
| sint32 | 使用可变长度编码 | int32 | int | int |
| sint64 | 使用可变长度编码 | int64 | long | int/long |
| fixed32 | 始终为四个字节 | uint32 | int | int/long |
| fixed64 | 始终为八个字节 | uint64 | long | int/long |
| sfixed32 | 始终为四个字节 | int32 | int | int |
| sfixed64 | 始终为八个字节 | int64 | long | int/long |
| bool |  | bool | boolean | bool |
| string | 字符串必须始终包含UTF-8编码或7位ASCII文本 | string | String | unicode(Python 2)/str(Python 3) |
| bytes | 可以包含任意字节序列 | string | ByteString | bytes |

标识号：在消息定义中，每个字段都有`唯一`的一个标识符。这些标识符是用来**在消息的二进制格式中识别各个字段的，一旦开始使用就不能够再改变**。

注：**[1, 15]之内的标识号在编码的时候会占用一个字节。[16, 2047]之内的标识号则占用2个字节**。所以应该为那些频繁出现的消息元素保留 [1, 15]之内的标识号。切记：要为将来有可能添加的、频繁出现的标识号预留一些标识号。

但是，在`proto3`中，对这些规则做了一些的修改：
* **取消了required字段修饰符，optional字段修饰符可以省略**；
* **移除了default选项**；
* **repeated字段默认采用packed编码**，即不需要明确使用[packed=true]来为字段指定比较紧凑的packed编码方式。

为什么移除default选项？

在`proto3`中，字段的默认值只能根据字段类型由系统决定。也就是说，默认值全部是约定好的，而不再提供指定默认值的语法。

在`proto2`中，若某字段被设置为默认值的时候，该字段不会被序列化。这样可以节省空间，提高效率。但这样就**无法区分某字段是根本没赋值，还是赋值了默认值**。也就是说，如果更新default默认值，会出现意想不到的问题。

为什么取消required字段修饰符？

因为required是`永久性`的：在将一个字段标识为required的时候，应该特别小心。如果在某些情况下不想写入或者发送一个required的字段，将原始该字段修饰符更改为optional可能会遇到问题——旧版本的使用者会认为不含该字段的消息是不完整的，从而可能会无目的的拒绝解析。

Google的一些工程师得出了一个结论：使用required弊多于利；他们更愿意使用optional和repeated而不是required。当然，这个观点并不具有普遍性。

也就是说，在`proto3`中，定义同样的message需要这样：
```proto
message Person {
    string name = 1;
    string phone = 2;
    string email = 3;
    repeated string address = 4;
}
```

当然，除了proto定义的数据类型之外，还可以指定自己定义的数据类型，甚至是枚举类型。

自己定义新的数据类型，只需要在.proto文件中定义新的message类型。枚举类型利用`enum`开头，需要注意**枚举类型的第一个字段的标识号必须为0**。

例如，在proto2中：
```proto
syntax = "proto2";

enum PhoneType {
    MOBILE = 0;
    HOME = 1;
    WORK = 2;
}

message PhoneNumber {
    required string number = 1;
    optional PhoneType type = 2;
}

message Address {
    optional string country = 1;
    optional string detail = 2;
}

message Person {
    required int32 id =1;
    required string name = 2;
    optional int32 age = 3;
    repeated string email = 4;
    repeated PhoneNumber phone = 5;
    optional Address address = 6;
}
```

在proto3中：
```proto
syntax = "proto3";

enum PhoneType {
    MOBILE = 0;
    HOME = 1;
    WORK = 2;
}

message PhoneNumber {
    string number = 1;
    PhoneType type = 2;
}

message Address {
    string country = 1;
    string detail = 2;
}

message Person {
    int32 id =1;
    string name = 2;
    int32 age = 3;
    repeated string email = 4;
    repeated PhoneNumber phone = 5;
    Address address = 6;
}
```

### 其他定义
proto可以通过**导入import**其他.proto文件中的定义来使用它们。即：
```proto
import proto路径
```

proto可以新增一个**可选的package声明符**，用来防止不同的消息类型有命名冲突。即：
```proto
package com.yngzmiao;
```

当然，proto除了这些定义规则之外，还有其他的规则。如`message嵌套定义`、`RPC服务接口`等等，一般情况下也不会使用到。需要了解的可以参考官方文档。

<br/>

## 相关阅读
* [Protobuf语言指南——.proto文件语法详解](https://blog.csdn.net/u014308482/article/details/52958148)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">