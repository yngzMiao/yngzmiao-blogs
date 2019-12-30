# include <iostream>

int main(int argc, char const *argv[])
{
  std::string str = "hello " + std::to_string(1);
  std::cout << str << std::endl;

  const char* const_char_str = str.c_str();
  std::cout << const_char_str << std::endl;

  char* char_str = const_cast<char*>(const_char_str);
  std::cout << char_str << std::endl;

  return 0;
}
