#include <iostream>
#include <memory>

class Frame {};

void fun(std::unique_ptr<Frame> f) {}

std::unique_ptr<Frame> getfun() {
  return std::unique_ptr<Frame>(new Frame());
}

void myDeleter(Frame* p)
{
  std::cout << "invoke deleter(Frame*)"<< std::endl;
  delete p;
}

int main()
{
  // std::unique_ptr<Frame> f(new Frame());
  // // std::unique_ptr<Frame> f1 = new Frame();
  // // std::unique_ptr<Frame> f2(f);
  // // std::unique_ptr<Frame> f3 = f;
  // // f1 = f;

  // std::unique_ptr<Frame> f4(std::move(new Frame()));
  // // std::unique_ptr<Frame> f5 = std::move(new Frame());
  // std::unique_ptr<Frame> f6(std::move(f4));
  // std::unique_ptr<Frame> f7 = std::move(f6);

  // std::unique_ptr<Frame[]> f8(new Frame[10]());

  // auto f9 = std::make_unique<Frame>(new Frame());

  // std::unique_ptr<Frame, decltype(&myDeleter)> f1(new Frame(), myDeleter);
  // auto del = [](Frame* p) {
  //   std::cout << "invoke deleter([](Frame *))"<< std::endl;
  //   delete p;
  // };
  // std::unique_ptr<Frame, decltype(del)> f2(new Frame(), del);

  // std::unique_ptr<Frame> f1(new Frame());
  // Frame* f2 = new Frame();
  // // fun(f1);
  // // fun(f2);
  // fun(std::move(f1));
  // std::unique_ptr<Frame> f3 = getfun();

  std::unique_ptr<Frame> f1(new Frame());
  Frame* f = f1.get();
  std::unique_ptr<Frame> f2;
  f2.reset(f1.release());
  f2.swap(f1);

  return 0;
}

