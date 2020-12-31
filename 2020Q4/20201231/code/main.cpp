#include <iostream>

class A {
  public:
    A() { a_ = 1; }
    ~A() { }
    int &GetRefA() {
      std::cout << "GetRefA()" << std::endl;
      return a_;
    }
    int GetValueA() {
      std::cout << "GetValueA()" << std::endl;
      return a_;
    }
    void fun() {
      std::cout << "fun()" << std::endl;
    }

    virtual void fun_v() {
      std::cout << "fun_v()" << std::endl;
    }

    int a_;
};

int main() {
  A *a = nullptr;
  // a->fun();
  a->GetRefA() = 2;
  // a->GetValueA();
  // a->fun_v();

  return 0;
}
