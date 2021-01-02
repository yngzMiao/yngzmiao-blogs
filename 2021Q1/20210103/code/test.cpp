#include <iostream>

class B;

class A {
  public:
    A() {
      aa_ = 'A';
    }
    char aa_;
    B *b_;
};

class B {
  public:
    B() {
      bb_ = 'B';
    }
    char bb_;
    A a_;
};

int main() {
  A tmp1;
  tmp1.b_ = new B();
  std::cout << tmp1.aa_ << " " << tmp1.b_->bb_ << std::endl;
  B tmp2;
  std::cout << tmp2.bb_ << " " << tmp2.a_.aa_ << std::endl;

  return 0;
}
