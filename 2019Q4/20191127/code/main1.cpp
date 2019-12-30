# include <iostream>
# include <sstream>

int main(int argc, char const *argv[])
{
  std::ostringstream oss;
  oss << "hello " << 2;
  std::string str = oss.str();
  std::cout << str << std::endl;

  oss.str("");
  oss << "hello " << 3.14159265357;
  oss.setf(std::ios::fixed);
  oss.precision(2);
  str = oss.str();
  std::cout << str << std::endl;

  oss.str("");
  oss.setf(std::ios::fixed);
  oss.precision(2);
  oss << "hello " << 3.14159265357;
  str = oss.str();
  std::cout << str << std::endl;

  return 0;
}
