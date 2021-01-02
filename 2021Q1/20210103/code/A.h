#ifndef A_H
#define A_H

class B;

class A {
  public:
    A() {
      aa_ = 'A';
    }
    void create();
    char aa_;
    B *b_;
};

#endif
