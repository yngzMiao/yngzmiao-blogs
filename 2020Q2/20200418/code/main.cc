#include <iostream>
#include <chrono>
#include <string.h>
#include <typeinfo>
#include <vector>

class MyString {
  public:
    static size_t DCtor;            // 默认构造函数
    static size_t Ctor;             // 构造函数
    static size_t CCtor;            // 拷贝构造函数
    static size_t CAsgn;            // 拷贝赋值
    static size_t MCtor;            // move构造
    static size_t MAsgn;            // move赋值
    static size_t Dtor;             // 析构函数
  private:
    char * _data;
    size_t _len;
    void _init_data(const char* s) {
      _data = new char[_len + 1];
      memcpy(_data, s, _len);
      _data[_len] = '\0';
    }
  
  public:
    MyString() : _data(NULL), _len(0) {++DCtor;}
    MyString(const char* p) : _len(strlen(p)) {
      ++Ctor;
      _init_data(p);
    }

    MyString(const MyString& str) : _len(str._len) {
      ++CCtor;
      _init_data(str._data);
    }
    MyString(MyString&& str) noexcept
      : _data(str._data), _len(str._len) {
        ++MCtor;
      str._len = 0;
      str._data = NULL;
    }

    MyString& operator= (const MyString& str) {
      ++CAsgn;
      if(_data != str._data) {
        if(_data)
          delete _data;
        _len = str._len;
        _init_data(str._data);
      }
      return *this;
    }
    MyString& operator= (MyString&& str) noexcept {
      ++MAsgn;
      if(_data != str._data) {
        if(_data)
          delete _data;
        _len = str._len;
        _data = str._data;
        str._len = 0;
        str._data = NULL;
      }
      return *this;
    }

    virtual ~MyString() {
      ++Dtor;
      if(_data)
        delete _data;
    }

    bool operator< (const MyString& rhs) const {        // 为了set
      return std::string(this->_data) < std::string(rhs._data);
    }
    bool operator== (const MyString& rhs) const {       // 为了set
      return std::string(this->_data) == std::string(rhs._data);
    }

    char* get() const {return _data;}
};

size_t MyString::DCtor = 0;
size_t MyString::Ctor = 0;
size_t MyString::CCtor = 0;
size_t MyString::CAsgn = 0;
size_t MyString::MCtor = 0;
size_t MyString::MAsgn = 0;
size_t MyString::Dtor = 0;

class MyStringNoMove {
  public:
    static size_t DCtor;            // 默认构造函数
    static size_t Ctor;             // 构造函数
    static size_t CCtor;            // 拷贝构造函数
    static size_t CAsgn;            // 拷贝赋值
    static size_t MCtor;            // move构造
    static size_t MAsgn;            // move赋值
    static size_t Dtor;             // 析构函数
  private:
    char * _data;
    size_t _len;
    void _init_data(const char* s) {
      _data = new char[_len + 1];
      memcpy(_data, s, _len);
      _data[_len] = '\0';
    }
  
  public:
    MyStringNoMove() : _data(NULL), _len(0) {++DCtor;}
    MyStringNoMove(const char* p) : _len(strlen(p)) {
      ++Ctor;
      _init_data(p);
    }

    MyStringNoMove(const MyStringNoMove& str) : _len(str._len) {
      ++CCtor;
      _init_data(str._data);
    }

    MyStringNoMove& operator= (const MyStringNoMove& str) {
      ++CAsgn;
      if(_data != str._data) {
        if(_data)
          delete _data;
        _len = str._len;
        _init_data(str._data);
      }
      return *this;
    }

    virtual ~MyStringNoMove() {
      ++Dtor;
      if(_data)
        delete _data;
    }

    bool operator< (const MyStringNoMove& rhs) const {        // 为了set
      return std::string(this->_data) < std::string(rhs._data);
    }
    bool operator== (const MyStringNoMove& rhs) const {       // 为了set
      return std::string(this->_data) == std::string(rhs._data);
    }

    char* get() const {return _data;}
};

size_t MyStringNoMove::DCtor = 0;
size_t MyStringNoMove::Ctor = 0;
size_t MyStringNoMove::CCtor = 0;
size_t MyStringNoMove::CAsgn = 0;
size_t MyStringNoMove::MCtor = 0;
size_t MyStringNoMove::MAsgn = 0;
size_t MyStringNoMove::Dtor = 0;

namespace std {
  template<>
  struct hash<MyString> {             // 为了unordered容器
    size_t operator() (const MyString& s) {
      return hash<string>()(string(s.get()));
    }
  };
  template<>
  struct hash<MyStringNoMove> {       // 为了unordered容器
    size_t operator() (const MyStringNoMove& s) {
      return hash<string>()(string(s.get()));
    }
  };
};

template<typename T>
void output_static_data(const T& myStr) {
  std::cout << typeid(myStr).name() << "--" << std::endl;
  std::cout << "CCtor = " << T::CCtor << " MCtor = " << T::MCtor
            << " CAsgn = " << T::CAsgn << " MAsgn = " << T::MAsgn
            << " Dtor = " << T::Dtor << " Ctor = " << T::Ctor
            << " DCtor = " << T::DCtor << std::endl;
}

template<typename T>
void test_moveable(T t, long value) {
  char buf[10];
  typedef typename std::iterator_traits<typename T::iterator>::value_type Vtype;

  std::chrono::milliseconds time1 = 
    std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch());

  for(long i = 0; i < value; ++i) {
    snprintf(buf, 10, "%d", rand());
    auto iter = t.end();
    t.insert(iter, Vtype(buf));
  }

  std::chrono::milliseconds time2 = 
    std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch());
  std::cout << "construction : " << (time2 - time1).count() << std::endl;
  output_static_data(*(t.begin()));

  T t1(t);
  std::chrono::milliseconds time3 = 
    std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch());
  std::cout << "copy : " << (time3 - time2).count() << std::endl;
  T t2(std::move(t));
  std::chrono::milliseconds time4 = 
    std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch());
  std::cout << "move : " << (time4 - time3).count() << std::endl;
}

template<typename T>
void process(T& i) {
  std::cout << "process(&) " << i << std::endl;
}

template<typename T>
void process(T&& i) {
  std::cout << "process(&&) " << i << std::endl;
}

template<typename T>
void forward(T&& i) {
  std::cout << "forward(&&) " << i << std::endl;
  process(std::forward<T>(i));
}

int main()
{
  test_moveable(std::vector<MyString>(), 10000000L);
  test_moveable(std::vector<MyStringNoMove>(), 10000000L);

  int c = 0;
  process(c);
  process(1);
  process(std::move(c));
  forward(2);
  forward(std::move(c));

  return 0;
}

