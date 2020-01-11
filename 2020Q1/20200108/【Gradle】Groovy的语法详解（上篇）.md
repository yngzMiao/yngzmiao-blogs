Gradle之所以在项目构建的过程中很强大，其中很大一部分原因就是在于Groovy。Grooovy是Java平台上设计的一种强大的、可选类型和动态的语言。Groovy可以无缝地集成任何Java程序，并立即让你的应用程序拥有强大的特性，包括脚本功能、DSL、运行时和编译时元编程和函数式编程。

也就是说，相比较于通过Maven等来进行项目构建，它可以更灵活多变地实现更加复杂的功能。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q1/20200108](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q1/20200108)。

<br/>

## 哪里书写gradle脚本
Gradle支持多Project的构建。多Project构建的结构包括：
* 项目的根目录下包含一个`settings.gradle`文件
* 项目的根目录下包含一个`build.gradle`文件
* 项目的子目录下包含自己的`build.gradle`文件
**settings.gradle文件的作用是告诉Gradle，项目和子项目是如何组织的。它是在Gradle构建的初始化阶段被执行的**。如果仅单个项目，该文件可以省略。

**build.gradle文件的作用是定义项目的具体内容和task的具体内容，因此也被称为构建脚本。它是在Gradle构建的配置/执行阶段被执行的**。

<br/>

## Groovy
### Groovy的特点
Groovy语言的特点：
* **基于Java虚拟机的一种灵活而动态的语言，可以无缝集成所有的Java类和库**；
* **既可以用于面向对象编程，又可以用作纯粹的脚本语言**；
* 具有Python、Ruby和Smalltalk等**动态语言中的特性(如动态类型转换、闭包和元编程)**；
* 支持DSL和其它紧凑的语法，使得代码易于阅读和维护。

### Groovy的下载与安装
需要注意的是，由于Gradle附带了自己的Groovy库，因此**如果已经安装了Gradle，就不需要安装Groovy**。

Groovy SDK的下载地址：[The Apache Groovy programming language](http://groovy-lang.org/download.html)。

这里介绍一下如何安装Groovy的二进制发行版(`Windows`)：
* 下载并解压Groovy的二进制发行版
* 设置GROOVY_HOME环境变量的值为解压的目录
* 添加GROOVY_HOME/bin路径到PATH环境变量中
* 设置JAVA_HOME环境变量的值为JDK的目录
安装完毕之后可以通过`groovy –version`命令来验证是否安装成功。如果安装成功，那么命令行将输出Groovy的版本号。

当然，如果是`Linux`操作系统的话，只需要在`~/.bashrc`文件中`export`下groovy可执行文件的位置即可。

<br/>

## Groovy语法
由于Groovy是一门语言，从头学习的话也需要好多章节，本博文就对其中经常需要用到的知识点进行一些梳理。**如果是和Java比较类似的语法，本博文就不做阐述了**。

其次，本博文采用的是**在build.gradle文件中进行书写，直接运行.gradlew文件即可进行编译和运行**。

### Hello World
之前提到，Groovvy可以无缝集成所有的Java类和库，既可以用于面向对象编程，又可以用作纯粹的脚本语言。这也就提供了`类方式`、`脚本方式`两种方式来编写Hello World程序。

类方式：所谓类方式，就是采用Java程序的编写方式。如：
```java
public class HelloWorld {
    public static void main(String[] args) {
        System.out.println("Hello World!");
    }
}
```

脚本方式：就是不需要按照类的这种较臃肿的方式进行编写，可以采用轻便紧凑的方式进行。如：
```java
System.out.println("Hello World!");
```

为了体现出Groovy构建项目的灵活性、快捷性，本文建议使用脚本方式进行编写。

可能你觉得打印的语句还不够精简，其实Groovy提供了更加方便快捷的写法：
```java
System.out.println("Hello World!");
println("Hello World!");                    //省略前缀
println "Hello World!";                     //省略括号
println "Hello World!"                      //省略分号
```

### Groovy省略规则
上面的例子可以看出，使用Groovy有时候可以省略很多东西，那么究竟Groovy可以省略什么东西呢？是不是在任意情况下都可以省略呢？

`可选的括号`：如果方法调用**至少有一个参数并且没有歧义**，那么可以省略括号。例如：
```java
println "Hello World"
```
但是，如果方法调用没有参数或者有歧义，那么需要圆括号。例如：
```java
println()                       //不能写成println
println(Math.max(5, 10))        //不能写成println Math.max 5, 10
```

`可选的分号`：如果**一行代码仅包含一条语句**，那么在该行末尾的分号可以省略；一行代码中的多个语句需要分号来分隔它们。

`可选的类型定义`：Groovy中的变量可以省略类型定义。例如：
```java
int x = 1
int y = 2
```
等同于下面这个脚本：
```java
x = 1
y = 2
```

`可选的return关键字`：在Groovy中，**一个方法或者闭包的最后一个表达式的值被返回**。因此，return关键字是可选的。例如：
```java
int add(int a, int b) {
    return a+b
}
println add(1, 2)
```

可以简化为：
```java
int add(int a, int b) {
    a+b
}
println add(1, 2)
```

`可选的public关键字`：**默认情况下，Groovy的类和方法都是public的**。例如：
```java
class Server {
    String toString() { 
        "a server" 
    }
}
```
等同于下面这个类：
```java
public class Server {
    public String toString() { 
        "a server" 
    }
}
```

### 变量定义与使用
Groovy支持可选类型。变量可以使用它们的具体类型或者def关键字来定义，当然，上文也提到Groovy可以省略类型定义。不过，博主建议使用def进行定义。
#### 字符串
Groovy的字符串支持`单引号`、`双引号`、`三引号`的定义方式。

单引号字符串和三引号字符串的区别在于：**单引号无格式，三引号有格式**。这一点和python的语法类似。例如：
```java
def s1 = 'hello world'
def s2 = '''hello

world'''

println s1
println s2
```
此时的输出为：
```bash
yngzmiao@yngzmiao-virtual-machine:~/test/test$ ./gradlew 
hello world
hello

world
```

那双引号字符串相较于单引号字符串，**增加了插值扩展表达式的功能**。插值是**将字符串中的占位符替换为字符串的值**，占位符表达式使用`${..}`来代表。如果没有插值表达式，那么双引号字符串是普通的java.lang.String。反之，如果有插值表达式，那么双引号字符串是groovy.lang.GString。**可以使用class属性来查看类型，或者使用instanceof表达式检验类型**。例如：
```java
def s1 = 'hello world'
def s2 = ">>> ${s1}"

println s2
```
此时的输出为：
```bash
yngzmiao@yngzmiao-virtual-machine:~/test/test$ ./gradlew 
>>> hello world
```

定义了字符串，就可以使用Groovy的一些对字符串操作的函数了，当然，Java中本身对于字符串的操作依旧有效。例如：
```java
def s1 = 'hello'
def s2 = 'hello world'
def s3 = '34'

s1.class                  //查看类型
s1 instanceof String      //判断类型
s1.compareTo(s2)          //字符串比较
s1 > s2                   //字符串比较
s1.charAt(1)              //提取字符
s1[2]                     //切片提取字符
s1[2..4]                  //切片提取字符，包含结束
s2.substring(1, 8)        //截取字符串，不包含结束
s2.minus(s1)              //字符串中去除子串
s2 - s1                   //字符串中去除子串
s2.replace(s1, s3)        //字符串代替
s2.indexOf(s1, 0)         //字符串查找
s1.reverse()              //字符串反转
s3.isNumber()             //字符串是否为数字
s3.toInteger()            //字符串转数字
```

#### 列表
Groovy**使用逗号分隔的值列表，用方括号括起来表示列表**。Groovy列表的类型是java.util.List，默认情况下，具体实现类是 java.util.ArrayList。例如：
```java
def numbers = [1, 2, 3]

numbers.class                   //查看类型
numbers instanceof List         //判断类型
numbers.get(2)                  //按索引获取元素
numbers.size()                  //获取列表长度
numbers << 4                    //添加元素
numbers.add(1, 5)               //按索引添加元素
numbers.remove(1)               //按索引删除元素
numbers.set(1, 5)               //按索引修改元素
numbers.contains(5)             //判断元素是否存在
numbers.indexOf(5)              //查找元素第一次出现位置
numbers.lastIndexOf(5)          //查找元素最后一次出现位置
```

提示一下，Groovy支持创建包含不同类型的值的列表。

#### Map
在Groovy中，初始化Map时，**将键和值用冒号分隔开来，每个键/值对用逗号分隔开来，并且全部的键和值用方括号括起来**。默认情况下，定义Map使用的具体实现类是 java.util.LinkedHashMap。例如：
```java
def colors = [red: '#FF0000', green: '#00FF00', blue: '#0000FF']

colors.class                        //查看类型
colors instanceof java.util.LinkedHashMap       //判断类型
colors['red']                       //按key获取元素，存在可修改，不存在可添加
colors.red                          //按key获取元素，存在可修改，不存在可添加
colors.get('red')                   //按key获取元素，存在可修改，不存在可添加
colors.remove("red")                //按key删除元素
colors.containsKey("red")           //判断key是否存在
colors.size()                       //获取map的长度
colors.keySet()                     //获取map的key集合
colors.entrySet()                   //获取map的键值对集合
colors.values()                     //获取map的value集合
```

#### 闭包
闭包是Groovy中的一个很重要的概念，由于篇幅问题，该部分内容留到下篇博文中进行介绍。

### 方法定义与使用
在Groovy中，方法定义有一个返回值类型，或者使用def关键字以使返回值无类型。方法还可以接收任意数量的参数，这些参数可以不显式声明它们的类型。Java的修饰符可以正常使用，如果没有提供修饰符，那么该方法是public的。

同时上文提到，如果Groovy中的方法需要返回值，但并没有提供返回语句，那么将返回执行的最后一行中的值。例如：
```java
def fun(person, age) {
  if(age > 18)
    println "hello adult ${person}"
  else
    println "hello child ${person}"
}

fun('zhangsan', 17)
fun('lisi', 19)
```

<br/>

## 相关阅读
* [Gradle Build Language Reference](https://docs.gradle.org/current/dsl/index.html)
* [Groovy 概述_w3cschool](https://www.w3cschool.cn/groovy/groovy_overview.html)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">
