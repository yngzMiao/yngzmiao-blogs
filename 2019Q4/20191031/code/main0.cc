# include <iostream>
# include <array>

int main(int argc, char const *argv[])
{
  std::array<int, 5> a0 = {0, 1, 2, 3, 4};          //正确
  std::array<int, 5> a1 = a0;                       //正确
  int m = 5;
  int b[m];                                 //正确，内置数组
  std::array<int, 5> a2;                    //正确
  // std::array<int, m> a3;                    //错误，array不可以用变量指定
  // std::array<int, 5> a4 = b;                //错误，array不可以用数组指定

  return 0;
}

