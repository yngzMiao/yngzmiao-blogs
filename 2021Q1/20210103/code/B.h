#ifndef B_H
#define B_H

#include "A.h"

class B {
  public:
    B() {
      bb_ = 'B';
    }
    char bb_;
    A a_;
};

#endif
