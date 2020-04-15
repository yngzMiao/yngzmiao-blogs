#include <iostream>
#include <string.h>

class MyStringNoDeep {
  private:
    char * _data;
    size_t _len;
    void _init_data(const char* s) {
      _data = new char[_len + 1];
      memcpy(_data, s, _len);
      _data[_len] = '\0';
    }
  
  public:
    MyStringNoDeep() : _data(NULL), _len(0) {}
    MyStringNoDeep(const char* p) : _len(strlen(p)) {
      _init_data(p);
    }

    virtual ~MyStringNoDeep() {
      delete _data;
    }

    char* get() const {return _data;}
};

class MyStringWithDeep {
  private:
    char * _data;
    size_t _len;
    void _init_data(const char* s) {
      _data = new char[_len + 1];
      memcpy(_data, s, _len);
      _data[_len] = '\0';
    }
  
  public:
    MyStringWithDeep() : _data(NULL), _len(0) {}
    MyStringWithDeep(const char* p) : _len(strlen(p)) {
      _init_data(p);
    }

    MyStringWithDeep(const MyStringWithDeep& str) : _len(str._len) {
      _init_data(str._data);
    }
    MyStringWithDeep& operator= (const MyStringWithDeep& str) {
      if(_data != str._data) {
        if(_data)
          delete _data;
        _len = str._len;
        _init_data(str._data);
      }
      return *this;
    }

    virtual ~MyStringWithDeep() {
      delete _data;
    }

    char* get() const {return _data;}
};

class MyString {
  private:
    char * _data;
    size_t _len;
    void _init_data(const char* s) {
      _data = new char[_len + 1];
      memcpy(_data, s, _len);
      _data[_len] = '\0';
    }
  
  public:
    MyString() : _data(NULL), _len(0) {}
    MyString(const char* p) : _len(strlen(p)) {
      _init_data(p);
    }

    MyString(const MyString& str) : _len(str._len) {
      std::cout << "MyString(&)" << std::endl;
      _init_data(str._data);
    }
    MyString(MyString&& str) noexcept
      : _data(str._data), _len(str._len) {
      std::cout << "MyString(&&)" << std::endl;
      str._len = 0;
      str._data = NULL;
    }

    virtual ~MyString() {
      if(_data)
        delete _data;
    }

    char* get() const {return _data;}
};

int main()
{
  int i = 0;
  int& j = i;               // 左值引用
  int&& k = 0;              // 右值引用

  char* buf = "Hello World";
  // MyStringNoDeep s = MyStringNoDeep(buf);
  // std::cout << s.get() << std::endl;

  MyStringWithDeep s = MyStringWithDeep(buf);
  std::cout << s.get() << std::endl;

  MyString s0(buf);
  MyString s1(s0);
  MyString s2(std::move(s0));
  MyString s3 = MyString(buf);

  return 0;
}