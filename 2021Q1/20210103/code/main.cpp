#include "B.h"
#include <iostream>

int main() {
  A tmp1;
  tmp1.create();
  std::cout << tmp1.aa_ << " " << tmp1.b_->bb_ << std::endl;
  B tmp2;
  std::cout << tmp2.bb_ << " " << tmp2.a_.aa_ << std::endl;

  return 0;
}
