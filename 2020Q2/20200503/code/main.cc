#include <iostream>
#include <memory>
#include <thread>

class Frame {};

class CB;
class CA {
  public:
    CA() {
      std::cout << "CA()" << std::endl;
    }
    ~CA() {
      std::cout << "~CA()" << std::endl;
    }
    void set_ptr(std::shared_ptr<CB>& ptr) {
      m_ptr_b = ptr;
    }
  private:
    std::shared_ptr<CB> m_ptr_b;
};

class CB {
  public:
    CB() {
      std::cout << "CB()" << std::endl;
    }
    ~CB() {
      std::cout << "~CB()" << std::endl;
    }
    void set_ptr(std::shared_ptr<CA>& ptr) {
      m_ptr_a = ptr;
    }
  private:
    std::weak_ptr<CA> m_ptr_a;
};

class Test {
  public:
    Test(int id) : m_id(id) {}
    void showID() {
      std::cout << m_id << std::endl;
    }
  private:
    int m_id;
};

void thread1(Test* t) {
  std::this_thread::sleep_for(std::chrono::seconds(2));
  t->showID();
}

void thread2(std::weak_ptr<Test> t) {
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::shared_ptr<Test> sp = t.lock();
  if(sp != nullptr)
    sp->showID();
  else
    std::cout << "error" << std::endl;
}

int main()
{
  // std::shared_ptr<CA> ptr_a(new CA());
  // std::shared_ptr<CB> ptr_b(new CB());
  // ptr_a->set_ptr(ptr_b);
  // ptr_b->set_ptr(ptr_a);
  // std::cout << ptr_a.use_count() << " " << ptr_b.use_count() << std::endl;

  // std::shared_ptr<Frame> f(new Frame());
  // std::weak_ptr<Frame> f1(f);
  // std::weak_ptr<Frame> f2 = f;
  // std::weak_ptr<Frame> f3(f1);
  // std::weak_ptr<Frame> f4 = f1;
  // std::weak_ptr<Frame> f5;
  // f5 = f;
  // f5 = f2;
  // std::cout << f.use_count() << std::endl;

  // Test* t = new Test(2);
  // std::thread t1(thread1, t);
  // delete t;
  // t1.join();

  std::shared_ptr<Test> sp = std::make_shared<Test>(2);
  std::thread t2(thread2, sp);
  // t2.join();
  t2.detach();

  return 0;
}