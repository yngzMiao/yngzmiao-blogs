# include <iostream>
# include <array>

int main(int argc, char const *argv[])
{
  std::array<int, 5> a = {0, 1, 2, 3, 4};
  std::cout << a.front() << " " << a.at(1) << " " << a[2] << " " << *(a.data() + 3) << " " << a.back() << std::endl;

  std::array<int, 5>::iterator iter;
  for (iter = a.begin(); iter != a.end(); ++iter)
    std::cout << *iter << " ";
  std::cout << std::endl;

  std::array<int, 5>::reverse_iterator riter;
  for (riter = a.rbegin(); riter != a.rend(); ++riter)
    std::cout << *riter << " ";
  std::cout << std::endl;

  return 0;
}

