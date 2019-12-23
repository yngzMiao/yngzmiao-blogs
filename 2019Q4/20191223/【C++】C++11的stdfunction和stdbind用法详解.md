在设计回调函数的时候，无可避免地会接触到可回调对象。在C++11中，提供了`std::function`和`std::bind`两个方法来对可回调对象进行统一和封装。

<br/>

## 可调用对象
 C++中有如下几种可调用对象：**函数、函数指针、lambda表达式、bind对象、函数对象**。其中，lambda表达式和bind对象是C++11标准中提出的(bind机制并不是新标准中首次提出，而是对旧版本中bind1st和bind2st的合并)。个人认为五种可调用对象中，函数和函数指针本质相同，而lambda表达式、bind对象及函数对象则异曲同工。

### 函数
这里的函数指的是普通函数，没什么可拓展的。

### 函数指针
插播一下函数指针和函数类型的区别：
* 函数指针指向的是函数而非对象。和其他指针类型一样，函数指针指向某种特定类型；
* **函数类型由它的返回值和参数类型决定，与函数名无关**。
例如：
```c
bool fun(int a, int b)
```
上述函数的函数类型是：`bool(int, int)`

上述函数的函数指针pf是：`bool (*pf)(int, int)`

一般对于函数来说，**函数名即为函数指针**：
```c
# include <iostream>

int fun(int x, int y) {                         //被调用的函数
    std::cout << x + y << std::endl;
	return x + y;
}

int fun1(int (*fp)(int, int), int x, int y) {   //形参为函数指针
	return fp(x, y);
}

typedef int (*Ftype)(int, int);                 //定义一个函数指针类型Ftype
int fun2(Ftype fp, int x, int y) { 
	return fp(x, y);
}

int main(){
	fun1(fun, 100, 100);                          //函数fun1调用函数fun
	fun2(fun, 200, 200);                          //函数fun2调用函数fun
}
```
编译并运行：
```bash
yngzmiao@yngzmiao-virtual-machine:~/test$ g++ main.cc -o main -std=C++11
yngzmiao@yngzmiao-virtual-machine:~/test$ ./main 
200
400
```
可以看出，**函数指针作为参数，可以在调用函数中调用函数指针代表的函数内容**。

### lambda表达式
lambda表达式就是一段可调用的代码。主要适合于只用到一两次的简短代码段。**由于lambda是匿名的，所以保证了其不会被不安全的访问**:
```c
# include <iostream>

int fun3(int x, int y){
	auto f = [](int x, int y) { return x + y; };  //创建lambda表达式,如果参数列表为空，可以省去() 
	std::cout << f(x, y) << std::endl;            //调用lambda表达式
}

int main(){
    fun3(300, 300);
}
```

关于lamdba表达式的内容，可以参考博文：[C++ 11 Lambda表达式](https://www.cnblogs.com/DswCnblog/p/5629165.html)。

### bind对象
std::bind可以用来生产，一个可调用对象来适应原对象的参数列表。具体的内容会在下文讲解。

### 函数对象
**重载了函数调用运算符`()`的类的对象，即为函数对象**。

<br/>

## std::function
由上文可以看出：由于可调用对象的定义方式比较多，但是函数的调用方式较为类似，因此需要使用一个统一的方式保存可调用对象或者传递可调用对象。于是，`std::function`就诞生了。

**std::function是一个可调用对象包装器，是一个类模板，可以容纳除了类成员函数指针之外的所有可调用对象，它可以用统一的方式处理函数、函数对象、函数指针，并允许保存和延迟它们的执行**。

定义function的一般形式：
```c
# include <functional>
std::function<函数类型>
```

例如：
```c
# include <iostream>
# include <functional>

typedef std::function<int(int, int)> comfun;

// 普通函数
int add(int a, int b) { return a + b; }

// lambda表达式
auto mod = [](int a, int b){ return a % b; };

// 函数对象类
struct divide{
    int operator()(int denominator, int divisor){
        return denominator/divisor;
    }
};

int main(){
	comfun a = add;
	comfun b = mod;
	comfun c = divide();
    std::cout << a(5, 3) << std::endl;
    std::cout << b(5, 3) << std::endl;
    std::cout << c(5, 3) << std::endl;
}
```

std::function可以取代函数指针的作用，因为它可以延迟函数的执行，特别适合作为`回调函数`使用。它比普通函数指针更加的灵活和便利。

故而，std::function的作用可以归结于：
1. **std::function对C++中各种可调用实体(普通函数、Lambda表达式、函数指针、以及其它函数对象等)的封装，形成一个新的可调用的std::function对象，简化调用**；
2. **std::function对象是对C++中现有的可调用实体的一种类型安全的包裹(如：函数指针这类可调用实体，是类型不安全的)**。

类型安全的介绍：[C++类型安全](https://www.cnblogs.com/Joe-BM/articles/2613245.html)

<br/>

## std::bind
std::bind可以看作一个通用的函数适配器，**它接受一个可调用对象，生成一个新的可调用对象来适应原对象的参数列表**。

std::bind将可调用对象与其参数一起进行绑定，绑定后的结果可以使用std::function保存。std::bind主要有以下两个作用：
* **将可调用对象和其参数绑定成一个仿函数**；
* **只绑定部分参数，减少可调用对象传入的参数**。

调用bind的一般形式：
```c
auto newCallable = bind(callable, arg_list);
```
该形式表达的意思是：当调用`newCallable`时，会调用`callable`，并传给它`arg_list`中的参数。

需要注意的是：arg_list中的参数可能包含形如_n的名字。其中n是一个整数，这些参数是占位符，表示newCallable的参数，它们占据了传递给newCallable的参数的位置。数值n表示生成的可调用对象中参数的位置：_1为newCallable的第一个参数，_2为第二个参数，以此类推。

直接文字可能不那么生动，不如看代码：
```c
#include <iostream>
#include <functional>

class A {
public:
    void fun_3(int k,int m) {
        std::cout << "print: k = "<< k << ", m = " << m << std::endl;
    }
};

void fun_1(int x,int y,int z) {
    std::cout << "print: x = " << x << ", y = " << y << ", z = " << z << std::endl;
}

void fun_2(int &a,int &b) {
    ++a;
    ++b;
    std::cout << "print: a = " << a << ", b = " << b << std::endl;
}

int main(int argc, char * argv[]) {
    //f1的类型为 function<void(int, int, int)>
    auto f1 = std::bind(fun_1, 1, 2, 3); 					//表示绑定函数 fun 的第一，二，三个参数值为： 1 2 3
    f1(); 													//print: x=1,y=2,z=3

    auto f2 = std::bind(fun_1, std::placeholders::_1, std::placeholders::_2, 3);
    //表示绑定函数 fun 的第三个参数为 3，而fun 的第一，二个参数分别由调用 f2 的第一，二个参数指定
    f2(1, 2);												//print: x=1,y=2,z=3
 
    auto f3 = std::bind(fun_1, std::placeholders::_2, std::placeholders::_1, 3);
    //表示绑定函数 fun 的第三个参数为 3，而fun 的第一，二个参数分别由调用 f3 的第二，一个参数指定
    //注意： f2  和  f3 的区别。
    f3(1, 2);												//print: x=2,y=1,z=3

    int m = 2;
    int n = 3;
    auto f4 = std::bind(fun_2, std::placeholders::_1, n); //表示绑定fun_2的第一个参数为n, fun_2的第二个参数由调用f4的第一个参数（_1）指定。
    f4(m); 													//print: a=3,b=4
    std::cout << "m = " << m << std::endl;					//m=3  说明：bind对于不事先绑定的参数，通过std::placeholders传递的参数是通过引用传递的,如m
    std::cout << "n = " << n << std::endl;					//n=3  说明：bind对于预先绑定的函数参数是通过值传递的，如n
    
    A a;
    //f5的类型为 function<void(int, int)>
    auto f5 = std::bind(&A::fun_3, &a, std::placeholders::_1, std::placeholders::_2); //使用auto关键字
    f5(10, 20);												//调用a.fun_3(10,20),print: k=10,m=20

    std::function<void(int,int)> fc = std::bind(&A::fun_3, a,std::placeholders::_1,std::placeholders::_2);
    fc(10, 20);   											//调用a.fun_3(10,20) print: k=10,m=20 

    return 0; 
}
```
编译并运行：
```bash
yngzmiao@yngzmiao-virtual-machine:~/test$ g++ main.cc -o main -std=C++11
yngzmiao@yngzmiao-virtual-machine:~/test$ ./main 
print: x = 1, y = 2, z = 3
print: x = 1, y = 2, z = 3
print: x = 2, y = 1, z = 3
print: a = 3, b = 4
m = 3
n = 3
print: k = 10, m = 20
print: k = 10, m = 20
```
由此例子可以看出：
* **预绑定的参数是以值传递的形式，不预绑定的参数要用std::placeholders(占位符)的形式占位，从_1开始，依次递增，是以引用传递的形式**；
* **std::placeholders表示新的可调用对象的第几个参数，而且与原函数的该占位符所在位置的进行匹配**；
* **bind绑定类成员函数时，第一个参数表示对象的成员函数的指针，第二个参数表示对象的地址，这是因为对象的成员函数需要有this指针**。并且编译器不会将对象的成员函数隐式转换成函数指针，需要通过&手动转换；
* **std::bind的返回值是可调用实体，可以直接赋给std::function**。

<br/>

## 相关阅读
* [C++中的可调用对象](https://blog.csdn.net/lc_910927/article/details/21250111)
* [C++函数指针、函数对象与C++11 function对象对比分析](https://blog.csdn.net/skillart/article/details/52336303)
* [bind原理图释](https://www.cnblogs.com/xusd-null/p/3698969.html)
* [C++11 中的std::function和std::bind](https://www.jianshu.com/p/f191e88dcc80)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">