#include <iostream>
#include <algorithm>
#include <array>

int main()
{
  std::array<int, 5> a1 = {4, 0, 2, 1, 3};
  std::array<int, 5> a2;

  std::sort(a1.begin(), a1.end());                  //排序函数
  for(int a: a1)
    std::cout << a << ' ';
  std::cout << std::endl;

  std::reverse(a1.begin(), a1.end());                           //反转a1
  for (std::array<int, 5>::iterator iter = a1.begin(); iter != a1.end(); ++iter)
    std::cout << *iter << " ";
  std::cout << std::endl;

  std::reverse_copy(a1.begin(), a1.end(), a2.begin());          //反转a1的内容拷贝到a2
  for (int i = 0; i < a2.size(); ++i)
    std::cout << a2[i] << " ";
  std::cout << std::endl;
}

