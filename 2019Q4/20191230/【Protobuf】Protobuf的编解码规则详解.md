在很多很多时候被问起，为什么选择protobuf？最先被想起的回答的就是**体积小、解析快**。那相比较于json、XML，为什么protobuf能够做到又小又快呢？

归其原因，这与它的编解码方式有很大的关系。本文将走进`protobuf`的深层原理来进行剖析。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191230](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191230)。

<br/>

## 实例
本文针对实际的例子，来对`protobuf`的编解码方式进行详细讲解。其中，`.proto`文件定义如下：
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
保存的person信息为：
```
id : 1
name : 'zhangsan'
age : 18
email : ['1.qq.com', '2.qq.com']
phone : [number: "123456", type: HOME, number: "234567", type: MOBILE]
address : country: "China", detail: "Jiangsu"
```
这段person信息，生成的一段二进制内容为：
```
08 01 12 08 7A 68 61 6E 67 73 61 6E 18 12 22 08 31 2E 
71 71 2E 63 6F 6D 22 08 32 2E 71 71 2E 63 6F 6D 2A 0A 
0A 06 31 32 33 34 35 36 10 01 2A 0A 0A 06 32 33 34 35 
36 37 10 00 32 10 0A 05 43 68 69 6E 61 12 07 4A 69 61 
6E 67 73 75
```

<br/>

## varint和ZigZag
在protobuf中，主要使用的两种编码方式就是`varint`和`ZigZag`。

### varint
`varint`是一种可变长编码，使用1个或多个字节对整数进行编码，可编码任意大的整数，小整数占用的字节少，大整数占用的字节多，如果小整数更频繁出现，则通过varint可实现压缩存储。

**varint中每个字节的最高位bit称之为most significant bit(MSB)，如果该bit为0意味着这个字节为表示当前整数的最后一个字节，如果为1则表示后面还有至少1个字节，可见，varint的终止位置其实是自解释的**。

也就是说，每个字节的最高位表示后面还有没有字节，若为0表示后面没有字节，若为1表示后面有字节。而**每个字节的低7位就是实际的值，并且使用小端的表示方法**。

例如1，varint的表示方法就为:
```
0000 0001
```
，而int本身是4字节的，采用varint的编码方式就省了三个字节。

再例如300，varint表示为:
```
10 0101100                          //300的二进制
10101100 00000010                   //300的varint编码
```
，而int本身是4字节的，采用varint的编码方式就省了两个字节。

### ZigZag
ZigZag编码：[整数压缩编码 ZigZag](https://www.cnblogs.com/en-heng/p/5570609.html)。

优秀的压缩编码应该满足：**高概率的码字字长应不长于低概率的码字字长**。而一般情况下，使用较多的是小整数，那么较小的整数应使用更少的byte来编码。基于此思想，ZigZag被提出来。

**ZigZag按绝对值升序排列，将整数hash成递增的32位bit流，其hash函数为h(n)=(n<<1) ^ (n>>31)；对应地long类型(64位)的hash函数为h(n)=(n<<1) ^ (n>>63)**。整数的补码(十六进制)与hash函数的对应关系如下：

| n | hex | h(n) | ZigZag(hex) |
| :---: | :---: | :---: | :---: |
| 0 | 00 00 00 00 | 00 00 00 00 | 00 |
| -1 | ff ff ff ff | 00 00 00 01 | 01 |
| 1 | 00 00 00 01 | 00 00 00 02 | 02 |
| -2 | ff ff ff fe | 00 00 00 03 | 03 |
| 2 | 00 00 00 02 | 00 00 00 04 | 04 |
| ... | ... | ... | ... |
| -64 | ff ff ff c0 | 00 00 00 7f | 7f |
| 64 | 00 00 00 40 | 00 00 00 80 | 80 | 01 |
| ... | ... | ... | ... |

可以看出，**Zigzag编码用无符号数来表示有符号数字，正数和负数交错**，这就是zigzag这个词的含义了。

其实，**正数扩大成2倍，负数取反加1**。

### 编码小结
其实varint编码和ZigZag，都可以编码正数和负数，那为什么protobuf怎么抉择的呢？

如果表示的都是正数，varint的方式编码会比ZigZag编码小很多；但**如果表示的很多都是负数，由于负数的最高位为1，如果负数也使用varint编码就会出现一个问题，int32总是需要5个字节，int64总是需要10个字节**。此时ZigZag的编码方式会更恰当。

为了统一两种方式，并效仿varint的压缩优势，减少ZigZag的字节数。最终，**sint32被编码为(n<<1) ^ (n>>31)对应的varint，sint64被编码为(n<<1) ^ (n>>63)对应的varint**，这样，绝对值较小的整数只需要较少的字节就可以表示。

因此，protobuf对于正数的编码采用varint，对于负数的编码采用ZigZag编码后的varint。

其实，这句话这样说是不恰当的。因为，protobuf也无法自动识别正数负数并做出不同的编码方式的选择。采用的做法是，**在.proto结构定义文件中，如果是int32、int64、uint32、uint64采用varint的方式，如果是sint32、sint64采用ZigZag编码后的varint的方式**。

<br/>

## protobuf编解码
对于序列化后字节流，需要回答的一个重要问题是从哪里到哪里是哪个数据成员。

因此**message的每个字段field在序列化时，一个field对应一个key-value对，整个二进制文件就是一连串紧密排列的key-value对，key也称为tag。采用这种key-value对的结构无需使用分隔符来分割不同的freld。对于可选的field，如果消息中不存在该field，那么在最终的message中就没有该field**，这些特性都有助于节约消息本身的大小。

key由wire type和FieldNumber两部分编码而成，具体地说，`key=(field_number<<3)|wire_type`，field_number部分指示了当前是哪个数据成员，通过它将cc和h文件中的数据成员与当前的key-value对应起来。也就是.proto文件中每个字段的标识号。

key的最低3个bit为wire type，什么是wire type？如下表所示：

| Type | Meaning | Used For |
| :---: | :---: | :---: |
| 0 | Varint | int32, int64, uint32, uint64, sint32, sint64, bool, enum |
| 1 | 64-bit | fixed64, sfixed64, double |
| 2 | Length-delimi | string, bytes, embedded messages, packed repeated fields |
| 3 | Start group | Groups (deprecated) |
| 4 | End group | Groups (deprecated) |
| 5 | 32-bit | fixed32, sfixed32, float |

由于key的第三位最多表示8个值，而wire type目前的种类是6种。由于采用varint的编码方式，只剩下4位的空闲存放field_number，因此之前在定义每个字段的标识号的时候建议不要超过15。

wire type被如此设计，主要是为了解决一个问题，如何知道接下来value部分的长度(字节数)，如果：
* **wire type=0、1、5，编码为key+数据，只有一个数据，可能占数个字节，数据在编码时自带终止标记**；
* **wire type=2，编码为key+length+数据，length指示了数据长度，可能有多个数据，顺序排序**。

需要注意的是：
* 如果出现**嵌套message，直接将嵌套message部分的编码接在length后即可**；
* **repeated后面接的字段，如果是个message，它重复出现多少次，编码时其key就会出现几次；如果接的是proto定义的字段，且以packed = true压缩存储时，只会出现1个key；如果不以压缩方式存储，其key也会出现多次**。在proto3中，默认以压缩方式进行存储，proto2中则需要显式地声明。

<br/>

## 阅读二进制文件
对于实例中的二进制文件，逐个解析：
```
08      // (1<<3)|0，1为id的field_bumber，0为id对应的wire type
01      // 0x01，id为1

12      // (2<<3)|1，2为name的field_bumber，1为name对应的wire type
08      // name字段的字符串长度
7A68616E6773616E      // "zhangsan"的ASCII码

18      // (3<<3)|0，3为age的field_bumber，0为age对应的wire type
12      // 0x12，age为18

22      // (4<<3)|2，4为email的field_bumber，2为email对应的wire type
08      // email字段的字符串长度
312E71712E636F6D      // "1.qq.com"的ASCII码
22      // (4<<3)|2，4为email的field_bumber，2为email对应的wire type
08      // email字段的字符串长度
322E71712E636F6D      // "2.qq.com"的ASCII码

2A      // (5<<3)|2，5为phone的field_bumber，2为phone对应的wire type
0A      // 0x10，phone的长度为10，1+1+6+1+1
0A      // (1<<3)|2，1为number的field_bumber，2为number对应的wire type
06      // number字段的字符串长度
313233343536      // "123456"的ASCII码
10      // (2<<3)|1，2为type的field_bumber，1为type对应的wire type
01      // enum为1，表示HOME

2A      // (5<<3)|2，5为phone的field_bumber，2为phone对应的wire type
0A      // 0x10，phone的长度为10，1+1+6+1+1
0A      // (1<<3)|2，1为number的field_bumber，2为number对应的wire type
06      // number字段的字符串长度
323334353637      // "234567"的ASCII码
10      // (2<<3)|1，2为type的field_bumber，1为type对应的wire type
00      // enum为0，表示MOBILE

32      // (6<<3)|2，6为address的field_bumber，2为address对应的wire type
10      // 0x10，address的长度为16，1+1+5+1+1+7
0A      // (1<<3)|2，1为country的field_bumber，2为country对应的wire type
05      // country字段的字符串长度
4368696E61      // "China"的ASCII码
12      // (2<<3)|2，2为detail的field_bumber，2为detail对应的wire type
07      // detail字段的字符串长度
4A69616E677375      // "Jiangsu"的ASCII码
```

<br/>

## protobuf的又小又快
### 数据变小一点
上文讲解的数据的varint编码方式，肯定能减少数据的大小，这点不再赘述。

诸如json、XML等数据，中间存在大量的冗余字符，比如{、}、"、<、>等等，为了减少数据量，我们可以暴力一点，直接把这些冗余信息去掉。但是会带来一些问题，就是当这段数据发送给接收端，接收端怎么知道每个value对应哪个key呢？

比较好的解决方案是：事先跟接收端约定好有哪些字段，顺序是什么，然后接收端按照这个规则对应起来。这就是.proto文件的内容。

但是，随之而来又有一个问题：.proto文件中的optional字段，如果没有没有该字段的信息，其实是不必要传递这个字段的。但此时在接收端，解析数据并按照顺序进行字段匹配的时候就会出问题。

显然已经乱套了，为了保证能够正确的配对，我们可以使用tag技术。也就是说，每个字段我们都用tag-value的方式来存储的，在tag当中记录两种信息，一个是value对应的字段的编号，另一个是value的数据类型(比如是整形还是字符串等)，因为tag中有字段编号信息，所以能够正确的配对。

可能你会问，使用tag的话，会增加额外的空间，这跟json的key-value有什么区别吗？

这个问题问的好，json中的key是字符串，每个字符就会占据一个字节，所以像name这个key就会占据4个字节，但在protobuf中，tag使用二进制进行存储，一般只会占据一个字节。相比较而言，tag所消耗的额外内存空间相对而言小很多。

即归纳成三点：
* **varint和ZigZag的编码方式**；
* **隔断冗余信息的剔除**；
* **tag-value方式的存储，tag采用二进制进行存储**。

### 解析变快一点
protobuf，它只需要简单地将一个二进制序列，按照指定的格式读取到C++对应的结构类型中就可以了。消息的decoding过程也可以通过几个位移操作组成的表达式计算即可完成。而对于字符串、自定义对象类型的数据，protobuf在存储的时候，也存储了该数据的字节长度，读取起来也非常快。

<br/>

## 相关阅读
* [Protocol Buffers（2）：编码与解码](https://blog.shinelee.me/2019/04-16-Protocol-Buffers%EF%BC%882%EF%BC%89%EF%BC%9A%E7%BC%96%E7%A0%81%E4%B8%8E%E8%A7%A3%E7%A0%81.html)
* [Google Protocol Buffer 的使用和原理](https://www.ibm.com/developerworks/cn/linux/l-cn-gpb/index.html)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">
