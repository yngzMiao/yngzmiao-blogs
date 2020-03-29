#include <iostream>

void f(int) {
  std::cout <<"invoke f(int)" << std::endl;
}

void f(void*) {
  std::cout << "invoke f(void*)" << std::endl;
}

int main(int argc, char *argv[]) {
	f(0);
  // f(NULL);

  void* px = NULL;
  // int* py = (void*)0;         //编译错误，不能隐式将void*转为int*类型
  int* pz = (int*)px;           //void*不能隐式转为int*，必须强制转换！

  int* pi = nullptr;            //ok！nullptr可以隐式转为任何其他指针类型
  void* pv = nullptr;           //ok! nullptr可以隐式转为任何其他指针类型

	return 0;
}