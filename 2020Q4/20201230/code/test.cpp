#include "test.h"

template<typename T>
void Rect<T>::display() {
  std::cout << left_ << " " << top_ << " " << right_
    << " " << bottom_ << std::endl;
}
