上文介绍了Groovy的一般省略规则，以及字符串、列表、Map的使用方式，然而在Groovy中，闭包是其中的一大杀器，可以对代码的结构和逻辑带来的优化显而易见。本博文主要对Groovy闭包的使用、文件IO的使用进行讲解。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q1/20200110](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q1/20200110)。

<br/>

## 闭包
**闭包，是一个代码块，或可以理解成一个匿名函数。在外部方法调用时，可以将其作为方法的实参传递给方法的形参，并在方法内部回调此匿名函数，且回调此匿名函数时可以传递实参给到匿名函数的内部去接收，并执行此匿名函数**。

同时，此代码块或匿名函数也可以赋值给一个变量，使其具有自执行的能力，且最后一行的执行语句作为匿名函数的返回。

### 闭包基础
可能乍一读并不太能够理解这段话的含义，可以先看几个例子：
```java
def b1 = {
    println "hello world"
}

b1()             //此处也可以写成b1.call()
```
运行后，输出结果为：
```bash
hello world
```
可以看出，所谓闭包，其实就是一段代码块(用大括号包括起来了)。它可以被赋值给一个闭包对象，如果查看此时b1的类型，其实是`Closure`。

既然存在闭包对象，那么很显然，也可以将其作为方法的实参传递给某个方法。例如：
```java
def b1 = {
    println "hello world"
}

def fun(Closure closure) {
  closure()
}

fun(b1)
```
可以看出，调用fun方法，在fun方法的实现里调用了闭包内容，在闭包内容中进行了打印语句的输出。

接下来，**闭包是可以进行参数传递的，且默认参数为it。当然，也可以显式地定义，通过`->`来进行定义**。例如：
```java
def b1 = {
    println "hello ${it}"           //默认参数it
}

def b2 = {
    def v ->                        //指定参数v
       println "hello ${v}"
}

def fun(Closure closure) {
  closure('world')
}

fun(b1)
fun(b2)
```
也就是说，闭包也是可以传递参数进行闭包内脚本程序地运行的。

到这边，可能你会发现，闭包实现的功能和方法(函数)的功能很像啊。那么闭包对象，实际上也就可以类比是C/C++的`函数指针`。那这有什么可炫耀的？

再看一个例子：
```java
def b1 = {
    def u, v ->                     //传递两个参数
        println "hello ${u} hello ${v}"
}

def fun(name1, name2, closure) {    //函数省略参数类型
  closure(name1, name2)
}

fun('zhangcan', 'lisi', b1)
```
运行后，输出结果为：
```bash
hello zhangcan hello lisi
```

根据上面的例子，可以将闭包放在函数的形参位置，整理一下格式：
```java
def fun(name1, name2, closure) {
  closure(name1, name2)
}

fun('zhangcan', 'lisi', {
    def u, v ->
        println "hello ${u} hello ${v}"
})
```

同时，**在Gradle中，如果闭包是方法的最后一个参数，那么闭包可以放在圆括号外面**。又可以整理成：
```java
def fun(name1, name2, closure) {
  closure(name1, name2)
}

fun('zhangcan', 'lisi') {
    def u, v ->
        println "hello ${u} hello ${v}"
}
```

是不是感觉，整个结构都变得特别紧凑。如果还是体会不到闭包带来的好处，在下文介绍Groovy对String、List、Map、fileTree等类型的闭包扩展，你会深刻体会到这种方式对代码书写带来的简单和快捷。

闭包最常用的情况是：**解放循环**。

### 闭包与数字
如果需要计算1+2+3+4+5+...+100的结果，可能你会：
```java
def result = 1
for (i in 1..100)
    result += i
println result
```
但如果使用闭包，可以写：
```java
def result = 1
1.upto(100) {
  result += it
}
println result
```

实际上，Number类型有一个`upto`方法，它有两个参数，第一个参数是另一个Number，第二个参数是一个闭包类型。在方法实现中，会遍历调用upto方法的值和第一个参数的值之间的数，并将其作为参数传递给闭包。那么，我们只需要在闭包中补充需要对那个数的操作即可。

同样的，除了upto方法之外，还有`downto`方法实现由大到小的遍历。
### 闭包与字符串
如果需要把字符串的每个字符打印出来，使用闭包的话，可以写成：
```java
def str = 'hello world'
str.each {
    println "char: ${it}"
}
```

实际上，String类型有一个`each`方法，它仅有一个参数，且此参数为一个闭包类型。在方法实现中，会遍历调用each方法的String对象的所有字符，并将其作为参数传递给闭包。由于闭包参数是仅有的参数，那么将其提出到圆括号外面，并省略方法的括号。

如果需要找出字符串中第一个满足某特定条件的字符(如第一个数字)，使用闭包的话，可以使用`find`方法写成：
```java
def str = 'hello 6 world'
println str.find {
    return it.isNumber()
}
```

需要注意的是，**此方法在闭包中需要返回一个布尔类型**。当返回值为true的时候，find方法将返回该字符。

### 闭包与列表
闭包对于列表，同样提供了许多方法。例如：
```java
def l = [6, 5, 8, 3, 7, 1, 4, 2, 0, 9]

println l.sort {                    //排序，由小到大排序
    def u, v ->
        u > v ? 1 : -1
}

println l.find {                    //找出第一个满足条件的列表元素
    return it % 2 == 0
}
println l.findAll {                 //找出所有满足条件的列表元素
    return it % 2 == 0
}

println l.any {                     //列表是否存在满足条件的元素
    return it % 2 == 0
}
println l.every {                   //列表是否都是满足条件的元素
    return it % 2 == 0
}
println l.count {                   //列表中满足条件的元素个数
    return it % 2 == 0
}

println l.max {                     //列表元素进行某运算操作后的最大值
    return Math.abs(it)
}
println l.min {                     //列表元素进行某运算操作后的最小值
    return Math.abs(it)
}
```

### 闭包与map
对map进行循环，应该是最常见的一种操作了。闭包对此提供了三种方式：
```java
def colors = ['red': '#FF0000', 'green': '#00FF00', 'blue': '#0000FF']

colors.each {
    println "key: ${it.key} value: ${it.value}"
    println "key: ${it.getKey()} value: ${it.getValue()}"
}

colors.eachWithIndex {
    def u, v->
        println "${v} key: ${u.key} value: ${u.value}"
}

colors.each {
    def u, v->
        println "key: ${u} value: ${v}"
}
```
总而言之，可以选择`each`方法、`eachWithIndex`方法。**当选择each方法时，可以选择闭包单参数，此时遍历map传入的参数就是键值对；也可以选择闭包双参数，此时遍历map传入的参数就是key和value。如果选择eachWithIndex方法时，闭包为双参数，第一参数是键值对，第二个参数是该键值对的索引值**。

闭包除了循环功能之外，还有其他的一些方法：
```java
def colors = ['red': '#FF0000', 'green': '#00FF00', 'blue': '#0000FF']

println colors.find {                   //找出第一个满足条件的元素键值对
    return it.key.equals('green')
}.collect{                              //对键值对进行过滤，返回输出内容
    return it.value
}

println colors.sort {                   //排序，由小到大排序
    def u, v ->
        return u.key > v.key ? 1 : -1
}

println colors.groupBy {                //对键值对进行条件分类
    return it.key.equals('blue') ? 'one' : 'two'
}             //结果：{two={red=#FF0000, green=#00FF00}, one={blue=#0000FF}}
```

<br/>

## 文件IO
Groovy提供了许多用于处理输入/输出的辅助方法。虽然可以在Groovy中使用标准的Java代码来处理这些问题，但是Groovy提供了更方便的方式来处理文件、流、阅读器等。例如，下面这些类中增加的方法：
* java.io.File类：[http://docs.groovy-lang.org/latest/html/groovy-jdk/java/io/File.html](http://docs.groovy-lang.org/latest/html/groovy-jdk/java/io/File.html)
* java.io.InputStream类：[http://docs.groovy-lang.org/latest/html/groovy-jdk/java/io/InputStream.html](http://docs.groovy-lang.org/latest/html/groovy-jdk/java/io/InputStream.html)
* java.io.OutputStream类：[http://docs.groovy-lang.org/latest/html/groovy-jdk/java/io/OutputStream.html](http://docs.groovy-lang.org/latest/html/groovy-jdk/java/io/OutputStream.html)
* java.io.Reader类：[http://docs.groovy-lang.org/latest/html/groovy-jdk/java/io/Reader.html](http://docs.groovy-lang.org/latest/html/groovy-jdk/java/io/Reader.html)
* java.io.Writer类：[http://docs.groovy-lang.org/latest/html/groovy-jdk/java/io/Writer.html](http://docs.groovy-lang.org/latest/html/groovy-jdk/java/io/Writer.html)
* java.nio.file.Path类：[http://docs.groovy-lang.org/latest/html/groovy-jdk/java/nio/file/Path.html](http://docs.groovy-lang.org/latest/html/groovy-jdk/java/nio/file/Path.html)

其实可以看出，这些类中增加的方法，基本都是闭包为参的方法。
### 读文件
打印文件的所有行，示例代码如下：
```java
def content = new File('test.txt')

println content.getText()               //获得文件内容字符串
println content.readLines()             //将文件每行内容作为元素，形成list列表
```

如果使用闭包类型为参数的情况下，可以使用：
```java
new File('test.txt').eachLine {
    println it
}

new File('test.txt').eachLine {
    def u, v ->
        println "line_no: ${v} line_content: ${u}"
}
```
`eachLine`方法的闭包单参数时，表示每行的内容；`eachLine`方法的闭包双参数时，第一个参数表示每行的内容，第二个参数表示行号。

同时，如果出于某种原因，**在eachLine方法体中抛出了一个异常，该方法将确保资源被正确地关闭。这个规则适用于Groovy添加的所有输入/输出资源方法**。也因此，在资源访问时，建议使用Groovy提供的闭包参数的方法。

当然，除了这个方法外，Groovy还提供了如下的方式：
```java
new File('test.txt').withReader {
    char[] buffer = new char[100]
    it.read(buffer)
    println buffer
}
```
### 写文件
利用闭包参数来写文件，代码也很简短清晰：
```java
new File('test1.txt').withWriter('utf-8') {
    it.writeLine 'hello world'
    it.append 'hello you'
}
```
### .properties文件
在Groovy的文件IO中，特意提一下`.properties`文件的打开方式：
```java
Properties prop = new Properties()
prop.load(new FileInputStream(new File('test.properties')))
println prop
```
### 文件树
```java
import groovy.io.FileType

new File('floder').eachFile {                 //遍历文件夹内的文件和目录
    println it.name
}

new File('floder').eachFileRecurse {          //递归遍历文件夹内的文件和目录
    println it.name
}

new File('floder').eachFileRecurse(FileType.FILES) {        //递归遍历文件夹内的文件
    println it.name
}
```

<br/>

## 相关阅读
* [Groovy Language Documentation](http://groovy-lang.org/single-page-documentation.html)
* [groovy 2.5.8 API](http://groovy-lang.org/api.html)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">