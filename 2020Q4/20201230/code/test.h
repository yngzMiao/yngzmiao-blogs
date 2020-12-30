#ifndef TEST_H
#define TEST_H

#include <iostream>

template<typename T>
class Rect {
  public:
    Rect(T l = 0.0f, T t = 0.0f, T r = 0.0f, T b = 0.0f) :
      left_(l), top_(t), right_(r), bottom_(b) {}

    void display();

    /*void display() {
      std::cout << left_ << " " << top_ << " " << right_
        << " " << bottom_ << std::endl;
    }*/

    T left_;
    T top_;
    T right_;
    T bottom_;
};

#endif
