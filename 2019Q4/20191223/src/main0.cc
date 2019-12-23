# include <iostream>
# include <functional>

typedef std::function<int(int, int)> comfun;

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

int fun3(int x, int y){
	auto f = [](int x, int y) { return x + y; };  //创建lambda表达式,如果参数列表为空，可以省去() 
	std::cout << f(x, y) << std::endl;            //调用lambda表达式
}

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
	fun1(fun, 100, 100);                          //函数fun1调用函数fun
	fun2(fun, 200, 200);                          //函数fun2调用函数fun
	fun3(300, 300);
	comfun a = add;
	comfun b = mod;
	comfun c = divide();
	std::cout << a(5, 3) << std::endl;
	std::cout << b(5, 3) << std::endl;
	std::cout << c(5, 3) << std::endl;
}

