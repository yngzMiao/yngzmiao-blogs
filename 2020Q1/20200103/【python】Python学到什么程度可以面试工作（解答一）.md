
逛知乎，看到帖子`Python学到什么程度可以面试工作？`，在`桃花岛主`的回答中讲到2019年最新的Python面试题，同时还有`旭东`大佬已做了大部分的解答。

博主会在几篇博文中，结合大佬的回答，对这些题目做更详尽的解答。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q1/20190103](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q1/20190103)。

<br/>

## 基础知识
1. 列出5个常用Python标准库？
```python
import os
import sys
import re
import math
import time
import datetime
import random
import threading
import multiprocessing
```

2. Python的内建数据类型有哪些？
```python
int、float、complex         # 数值型
bool            # 布尔型
str             # 字符串
list            # 列表
tuple           # 元组
dict            # 字典
```

3. 简述with方法打开处理文件帮我我们做了什么？
* with语句适用于**对资源进行访问的场合，确保不管使用过程中是否发生异常都会执行必要的清理操作，释放资源**，比如文件使用后自动关闭、线程中锁的自动获取和释放等；
* with语句即`上下文管理器`，在程序中用来表示代码执行过程中所处的前后环境。上下文管理器：含有`__enter__`和`__exit__`方法的对象就是上下文管理器。 
* __enter__()：在执行with语句之前，首先执行该方法，通常返回一个实例对象，如果with语句有as目标，则将对象赋值给as目标。 
* __exit__()：在执行with语句结束后，自动调用__exit__()方法，用户释放资源，若**此方法返回布尔值True，程序会忽略异常**。 
* 使用环境：文件读写、线程锁的自动释放等。
```python
with context_expression [as target(s)]:
    with-body
```
这里context_expression要**返回一个上下文管理器对象，该对象并不赋值给as子句中的target(s)，而是会将上下文管理器的__enter__()方法的返回值赋值给target(s)**。

不太清楚的可以参考链接：[深入理解 Python 中的上下文管理器](https://juejin.im/post/5c87b165f265da2dac4589cc)。

4. Python的可变和不可变数据类型？
* 不可变数据类型：即数据被创建之后，数据的值将不再发生改变，有数值、字符、元祖类型；
* 可变数据类型：数据别创建之后，数据的值可以发生变化，有列表、字典、集合类型。

5. Python获取当前日期？
```python
# -*- coding: UTF-8 -*-
import datetime
import time

if __name__ == "__main__":
    print(time.time())           # 时间戳
    print(time.strftime("%Y-%m-%d %H:%M:%S %w", time.localtime()))  # 年月日时分秒
    print(datetime.datetime.now())      # 年月日时分秒
```

6. 统计字符串每个单词出现的次数。
```python
def word_amount(sentence):
    split_list = sentence.split()
    dict_result = {}
    for word_name in split_list:
        if word_name not in dict_result.keys():
            dict_result[word_name] = 1
        else:
            dict_result[word_name] += 1
    return dict_result

if __name__ == '__main__':
    sentence = "I can because i think i can"
    dict_result = word_amount(sentence)
    print(dict_result)
```
或者：
```python
if __name__ == '__main__':
    sentence = "I can because i think i can"
    result = {word: sentence.split().count(word) for word in set(sentence.split())}
    print(result)
```
或者：
```python
from collections import Counter

if __name__ == '__main__':
    sentence = "I can because i think i can"
    counts = Counter(sentence.split())
    print(counts)
```

7. 用python删除文件和用linux命令删除文件方法。
```python
import os
os.remove("demo.txt")
```
```bash
rm demo.txt
```

8. 写一段自定义异常代码？
```python
class printException(Exception):
    pass

def testRaise():
    raise printException('printErr')

if __name__ == '__main__':
    try:
        testRaise()
    except printException, e:
        print e
```

9. 举例说明异常模块中try except else finally的相关意义。
```python
# -*- coding: UTF-8 -*-

def read_filedata(file_name):
    file_obj = ""
    try:
        # 需要检测的异常代码片段
        file_obj = open(file_name, "r")
        result_data = file_obj.read()
    except IOError, e:
        # 发生“IOError”异常进行处理的代码片段
        file_obj = "文件不存在："+ str(e)
    else:
        # 没有引发“IOError”异常执行的代码片段，返回读取到的数据
        return result_data
    finally:
        # 不管有没有引发错误都会执行的代码片段，isinstance()用于判断一个数据类型
        if isinstance(file_obj, str):
            return file_obj
        elif isinstance(file_obj, file):
            file_obj.close()
        else:
            return "未知错误，请检查您的代码..."

if __name__ == '__main__':
    result = read_filedata("abc.txt")
    print(result)
```

10. 遇到 bug 如何处理？
* 首先查看报错信息，根据报错信息找到相应代码，通常一般的数据结构或者算法错误只要找到报错代码就可以顺利解决；
* 如果遇到暂时不能解决的错误先不要慌，我们可以使用编译器的Debug模式或者自己在代码中加注断点进行代码排查；
* 如果依然不能解决bug，我们可以拷贝报错信息，在搜索引擎中进行搜索。 没有人写代码不出bug，如果你在一个bug上耗费时间超过半小时，可以与其他同事探讨(注意节制，可能有些费同事)；
* 另辟蹊径：方法总比困难多，在进行快速开发时，我们应该优先实现功能而不是拘泥于运行效率，所以遇到一些暂时不能解决的BUG可以考虑另外的实现方法。

<br/>

## 语言特性
1. 谈谈对Python的了解和其他语言的区别？

Python是一门语法简洁优美，功能强大无比，应用领域非常广泛，具有强大完备的第三方库的一门`强类型`的`动态`，可移植，可扩展，可嵌入的`解释型`编程语言。

强类型语言、弱类型语言的区别：
* 如果语言经常隐式地转换变量的类型，那这个语言就是`弱类型语言`，如果很少会这样做，那就是`强类型语言`。Python很少会隐式地转换变量的类型，所以Python是强类型的语言。
* 强类型语言和弱类型原因其判断的根本是**是否会隐式进行语言类型转变。强类型原因在速度上可能略逊于弱类型语言，但是强类型定义语带来的严谨性又避免了不必要的错误**。
* 强类型语言包括：Java、.net、Python、C++等语言。其中Python是动态语言，是强类型定义语言，是类型安全的语言，Java是静态语言，是强类型定义语言，也是类型安全的语言；弱类型语言包括：VB，PHP，JavaScript等语言。其中VBScript是动态语言，是一种类型不安全的原因。

动态语言、静态语言的区别：
* `动态类型语言`：**动态性语言是指在运行期间才去做数据类型检查的语言**，也就是说动态类型语言编程时，永远不用给任何变量指定数据类型，该语言会在第一次赋值给变量时，在内部将数据类型记录下来**。Python和Ruby就是一种典型的动态类型语言，其他的各种脚本语言如VBScript也多少属于动态类型语言。
* `静态类型语言`：静态类型语言与动态类则刚好相反，**它的数据类型在编译期间检查，也就是说在写程序时要声明所有变量的数据类型**，C/C++是静态类型语言的典型代表，其他静态语言还有C#、Java等。
* 对于动态语言与静态语言的区分，其根本在于**数据类型是在运行期间检查还是在编译期间检查**。

编译型语言、解释型语言的区别：
* `编译型语言`：**需要将一段程序直接翻译成机器码(对于C/C++这种非跨平台的语言)或者中间码(Java这种跨平台语言，需要虚拟机再将中间码印射成机器码)**。一般需经过编译(compile)、链接(linker)这两个步骤。编译是把源代码编译成机器码，链接是把各个模块的机器码和依赖库串连起来生成可执行文件。
* `解释型语言`：**使用解释器将源码逐行解释成机器码并立即执行**，不会进行整体性的编译和链接处理，相比编译型语言省了道工序。
* 一个像是吃饭等菜都上全了再开动，一个像是吃火锅，边涮边吃，时机不一样。
* 解释型语言的优点：跨平台容易，只需提供特定平台的解释器；缺点：每次运行的时候都要解释一遍，性能上不如编译型语言。

2. 简述解释型和编译型编程语言？

可参考上一条的解释内容。

3. Python的解释器种类以及相关特点？
* `CPython`：**官方版本的解释器**。这个解释器是用C语言开发的，所以叫CPython。在命令行下运行python就是启动CPython解释器。 CPython是使用最广的Python解释器。
* `IPython`：IPython是基于CPython之上的一个交互式解释器，也就是说，IPython只是在交互方式上有所增强，但是执行Python代码的功能和CPython是完全一样的。CPython用>>>作为提示符，而IPython用In [序号]:作为提示符。
* `PyPy`：它的**目标是执行速度。PyPy采用JIT技术，对Python代码进行动态编译(注意不是解释)**，所以可以显著提高Python代码的执行速度。绝大部分Python代码都可以在PyPy下运行，但是PyPy和CPython有一些是不同的，这就导致相同的Python代码在两种解释器下执行可能会有不同的结果。
* `Jython`：Jython是运行在Java平台上的Python解释器，可以直接把Python代码编译成Java字节码执行。
* `IronPython`：IronPython和Jython类似，只不过IronPython是运行在微软.Net平台上的Python解释器，可以直接把Python代码编译成.Net的字节码。

4. 说说你知道的Python3和Python2之间的区别？

`编码`：**Python2的默认编码是asscii**，这也是导致Python2中经常遇到编码问题的原因之一，至于是为什么会使用asscii作为默认编码，原因在于Python这门语言诞生的时候还没出现Unicode。**Python3默认采用了UTF-8作为默认编码**，因此你不再需要在文件顶部写# coding=utf-8了。

`字符串`：Python2中字符的类型，**str：已经编码后的字节序列，unicode：编码前的文本字符**；而Python3中字符的类型，**str：编码过的unicode文本字符，bytes：编码前的字节序列**。

可以认为字符串有两种状态，即文本状态和字节(二进制)状态。Python2和Python3中的两种字符类型都分别对应这两种状态，然后相互之间进行编解码转化。编码就是将字符串转换成字节码，涉及到字符串的内部表示；解码就是将字节码转换为字符串，将比特位显示成字符。

在Python2中，str和unicode都有encode和decode方法。但是不建议对str使用encode，对unicode使用decode, 这是Python2设计上的缺陷。Python3则进行了优化，str只有一个encode方法将字符串转化为一个字节码，而且bytes也只有一个decode方法将字节码转化为一个文本字符串。

`print`：**Python2中的print是语句；Python3中的print是函数**。例如：
```python
# py2
>>> print("hello", "world")
('hello', 'world')

# py3
>>> print("hello", "world")
hello world
```

这个例子就比较明显了，在py2中，print语句后面接的是一个元组对象，而在py3中，print函数可以接收多个位置参数。如果希望在Python2中把print当函数使用，那么可以导入future模块中的print_function。

`import`：**python2默认是按照相对路径导入模块和包，python3默认则是按照绝对路径导入**。

import的理解：[python2和python3 通过import导入模块和包的区别](https://blog.csdn.net/Blateyang/article/details/89492570)

`input`：Python3：input解析输入为str字符型；Python2：input解析输入为int型，raw_input解析输入为str类型。

`算法符`：在Python2中，**/执行传统除法，对于整数执行截断除法，浮点数执行浮点除法(保留小数部分，即使整除)**；//执行Floor除法，截除掉余数并且针对整数操作数返回一个整数，如果有任何一个操作数是浮点数，则返回一个浮点数。在Python3中，**/总是执行真除法，不管操作数的类型，都会返回包含任何余数的浮点结果**；//执行Floor除法，截除掉余数并且针对整数操作数返回一个整数，如果有任何一个操作数是浮点数，则返回一个浮点数。

`int/long`：**Python3里，只有一种整数类型int**，大多数情况下，它很像Python2里的长整型。**Python2有为非浮点数准备的int和long类型**。int类型的最大值不能超过sys.maxint，而且这个最大值是平台相关的。

`True和False`：**在Python2中，True和False是两个全局变量(名字)，在数值上分别对应1和0**，既然是变量，那么它们就可以指向其它对象。**Python3修正了这个缺陷，True和False变为两个关键字**，永远指向两个固定的对象，不允许再被重新赋值。

`迭代器`：**在Python2中很多返回列表对象的内置函数和方法在Python3都改成了返回类似于迭代器的对象，因为迭代器的惰性加载特性使得操作大数据更有效率**。

例如：**Python2中使用xrange()来创建一个迭代器对象，使用range()创建一个list数组(要生成很大的数字序列的时候，用xrange会比range性能优很多，因为不需要一上来就开辟一块很大的内存空间)；Python3中使用range()创建迭代器对象，移除了xrange()方法**。

另外，字典对象的dict.keys()、dict.values()方法都不再返回列表，而是以一个类似迭代器的view对象返回。高阶函数map、filter、zip返回的也都不是列表对象了。Python2的迭代器必须实现next方法，而Python3改成了__iter__()、__next__。

`nonlocal`：在Python2中可以在函数里面可以用关键字global声明某个变量为全局变量，但是在嵌套函数中，想要给一个变量声明为非局部变量是没法实现的，在Pyhon3，新增了关键字nonlcoal，一般使用在闭包中的，使变量使用外层的同名变量。

`LEGB`作用域的理解：[python3的local, global, nonlocal简析](https://blog.csdn.net/lxy210781/article/details/81139493)

5. Python3和Python2中int和long区别？

可参考上一条的解释内容。

6. xrange和range的区别？

可参考上上一条的解释内容。

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">
