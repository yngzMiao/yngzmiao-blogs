上一篇博文讲解了`protobuf`的安装和`.proto`文件的定义，并且可以生成C++版本的.cc和.h文件，python的.py文件。那么本文就利用生成的这些数据访问类，进行对象的序列化和反序列化。

上篇博文地址：[【Protobuf】Protobuf下载安装和.proto文件定义](https://blog.csdn.net/qq_38410730/article/details/103679529)

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191228](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191228)。

<br/>

## .proto文件定义
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

<br/>

## C++版本
### 认识数据访问类
查看生成的`person.pb.h`，会看到**在.proto文件中指定的每条message都有一个类。而且每个message的字段，都被定义成类中的成员变量，并且还提供了一些方法对这些字段进行修改等操作**。

以上文的.proto为例，每条message都对应设置为一个类，枚举message也同样被设置了：
```c
class PhoneNumber;
class Address;
class Person;

enum PhoneType {
  MOBILE = 0,
  HOME = 1,
  WORK = 2
};
```
以Person为例，查看类中的几个成员变量，囊括了message中定义的字段：
```c
::std::string* name_;
::google::protobuf::int32 id_;
::google::protobuf::int32 age_;
::google::protobuf::RepeatedPtrField< ::std::string> email_;
::google::protobuf::RepeatedPtrField< ::PhoneNumber > phone_;
::Address* address_;
```
对message中的这些字段，person.pb.h也提供了一些方法来进行修改等操作：
```c
// required int32 id = 1;
inline bool has_id() const;
inline void clear_id();
static const int kIdFieldNumber = 1;
inline ::google::protobuf::int32 id() const;
inline void set_id(::google::protobuf::int32 value);

// required string name = 2;
inline bool has_name() const;
inline void clear_name();
static const int kNameFieldNumber = 2;
inline const ::std::string& name() const;
inline void set_name(const ::std::string& value);
inline void set_name(const char* value);
inline void set_name(const char* value, size_t size);
inline ::std::string* mutable_name();
inline ::std::string* release_name();
inline void set_allocated_name(::std::string* name);

// optional int32 age = 3;
inline bool has_age() const;
inline void clear_age();
static const int kAgeFieldNumber = 3;
inline ::google::protobuf::int32 age() const;
inline void set_age(::google::protobuf::int32 value);

// repeated string email = 4;
inline int email_size() const;
inline void clear_email();
static const int kEmailFieldNumber = 4;
inline const ::std::string& email(int index) const;
inline ::std::string* mutable_email(int index);
inline void set_email(int index, const ::std::string& value);
inline void set_email(int index, const char* value);
inline void set_email(int index, const char* value, size_t size);
inline ::std::string* add_email();
inline void add_email(const ::std::string& value);
inline void add_email(const char* value);
inline void add_email(const char* value, size_t size);
inline const ::google::protobuf::RepeatedPtrField< ::std::string>& email() const;
inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_email();

// repeated .PhoneNumber phone = 5;
inline int phone_size() const;
inline void clear_phone();
static const int kPhoneFieldNumber = 5;
inline const ::PhoneNumber& phone(int index) const;
inline ::PhoneNumber* mutable_phone(int index);
inline ::PhoneNumber* add_phone();
inline const ::google::protobuf::RepeatedPtrField< ::PhoneNumber >&
    phone() const;
inline ::google::protobuf::RepeatedPtrField< ::PhoneNumber >*
    mutable_phone();

// optional .Address address = 6;
inline bool has_address() const;
inline void clear_address();
static const int kAddressFieldNumber = 6;
inline const ::Address& address() const;
inline ::Address* mutable_address();
inline ::Address* release_address();
inline void set_allocated_address(::Address* address);
```
对message中字段进行修改等操作的方法，大致可以看出如下规律：
* **对于基本proto类型，非repeated字段，提供了与字段名相同的方法名来获取该字段的内容，提供了set方法来设置该字段的内容；而repeated字段，则需要根据index来获取字段的内容，提供了add方法来添加该字段的内容，或者指定index来进行set**；
* **对于自定义的message类型，非repeated字段，提供了与字段名相同的方法名来获取该字段的内容，提供了mutable方法来设置该字段的内容；而repeated字段，则需要根据index来获取字段的内容，提供了add方法来添加该字段的内容，或者指定index来进行mutable**。

### 序列化、反序列化
对于序列化和反序列化的方法，都定义在`message.h`中：
```c
bool ParseFromIstream(std::istream* input);
bool SerializeToOstream(std::ostream* output) const;
bool ParseFromString(const std::string& data);
bool SerializeToString(std::string* output) const;
bool SerializeToArray(void* data, int size) const;

std::string GetTypeName() const override;
size_t ByteSizeLong() const override;
```

### 实例：
实例内容：将`person`的信息利用`protobuf`，序列化保存到文件`main.proto`(这里的.proto文件是二进制文件，和.proto结构声明文件仅仅后缀名相同而已)中；再读取该文件，反序列化成对象打印出来。
```c
#include <iostream>
#include <fstream>
#include "person.pb.h"

int main(int argc, char const *argv[])
{
  Person *person = new Person();
  person->set_id(1);
  person->set_name("zhangsan");
  person->set_age(18);
  person->add_email("1.qq.com");
  person->add_email("2.qq.com");
  PhoneNumber *phone1 = person->add_phone();
  phone1->set_number("123456");
  phone1->set_type(PhoneType::HOME);
  PhoneNumber *phone2 = person->add_phone();
  phone2->set_number("234567");
  phone2->set_type(PhoneType::MOBILE);
  Address *address = person->mutable_address();
  address->set_country("China");
  address->set_detail("Jiangsu");

  std::cout << "write to main.proto" << std::endl;
  std::ofstream ofs;
  ofs.open("main.proto", std::ofstream::binary);
  person->SerializeToOstream(&ofs);
  ofs.close();

  std::cout << "read from main.proto" << std::endl;
  Person *person_tmp = new Person();
  std::ifstream ifs;
  ifs.open("main.proto", std::ofstream::binary);
  person_tmp->ParseFromIstream(&ifs);
  std::cout << "id : " << person_tmp->id() << std::endl;
  std::cout << "name : " << person_tmp->name() << std::endl;
  std::cout << "age : " << person_tmp->age() << std::endl;
  std::cout << "email : " << person_tmp->email(0) << " " << person_tmp->email(1) << std::endl;
  std::cout << "phone : " << person_tmp->phone(0).number() << " " << person_tmp->phone(1).number() << std::endl;
  std::cout << "address : " << person_tmp->address().country() << " " << person_tmp->address().detail() << std::endl;
  return 0;
}
```
采用`CMakeLists.txt`进行编译并运行：
```bash
yngzmiao@yngzmiao-virtual-machine:~/test/c++$ ./main 
write to main.proto
read from main.proto
id : 1
name : zhangsan
age : 18
email : 1.qq.com 2.qq.com
phone : 123456 234567
address : China Jiangsu
```

<br/>

## Python版本
与C++版本类似，具体的`person_pb2.py`就不对具体的数据访问类进行分析了，想要了解，可以自己解读下。

### 序列化、反序列化
对于序列化和反序列化的方法，也比较简单：
```c
str = obj.SerializeToString()
obj.ParseFromString(str)

obj.ByteSize()
```

### 实例：
实例内容：将`person`的信息利用`protobuf`，序列化保存到文件`main.proto`(这里的.proto文件是二进制文件，和.proto结构声明文件仅仅后缀名相同而已)中；再读取该文件，反序列化成对象打印出来。
```c
# -*- coding:UTF-8 -*-
import os,sys
import proto_pb2.person_pb2 as person_proto

if __name__ == "__main__":
  person = person_proto.Person()
  person.id = 1
  person.name = "zhangsan"
  person.age = 18

  person.email.append("1.qq.com")
  person.email.append("2.qq.com")
  phone1 = person.phone.add()
  phone2 = person.phone.add()
  phone1.number = "123456"
  phone1.type = person_proto.PhoneType.HOME
  phone2.number = "234567"
  phone2.type = person_proto.PhoneType.MOBILE
  addr = person.address
  addr.country = "China"
  addr.detail = "Jiangsu"

  print("write to main.proto")
  fw = open("main.proto", "w")
  fw.write(person.SerializeToString())
  fw.close()

  print("read from main.proto")
  fr = open("main.proto", "r")
  data = fr.read()
  person_tmp = person_proto.Person()
  person_tmp.ParseFromString(data)
  print("id : " + str(person_tmp.id))
  print("name : " + str(person_tmp.name))
  print("age : " + str(person_tmp.age))
  print("email : " + str(person_tmp.email))
  print("phone : " + str(person_tmp.phone))
  print("address : " + str(person_tmp.address))
  fr.close()
```
运行该脚本：
```bash
yngzmiao@yngzmiao-virtual-machine:~/test/python$ python main.py
write to main.proto
read from main.proto
id : 1
name : zhangsan
age : 18
email : [u'1.qq.com', u'2.qq.com']
phone : [number: "123456"
type: HOME
, number: "234567"
type: MOBILE
]
address : country: "China"
detail: "Jiangsu"
```

<br/>

## 二进制文件
对比一下C++版本和python版本各自生成的`main.proto`二进制文件，最终发现，**两个文件一模一样**。

打开生成的二进制文件main.proto，可以看到内容：
```
08 01 12 08 7A 68 61 6E 67 73 61 6E 18 12 22 08 31 2E 
71 71 2E 63 6F 6D 22 08 32 2E 71 71 2E 63 6F 6D 2A 0A 
0A 06 31 32 33 34 35 36 10 01 2A 0A 0A 06 32 33 34 35 
36 37 10 00 32 10 0A 05 43 68 69 6E 61 12 07 4A 69 61 
6E 67 73 75
```
这些二进制文件是什么含义呢，会在后面的博文中具体介绍。

<br/>

## 相关阅读
* [Developer Guide](https://developers.google.com/protocol-buffers/docs/overview)
* [protobuf相关的操作函数](https://blog.csdn.net/cws1214/article/details/48525659)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">
