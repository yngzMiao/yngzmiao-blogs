#include <iostream>
#include <vector>
#include <complex>

class P {
  public:
    P() {
      std::cout << "P" << std::endl;
    }
    P(int a, int b) {
      std::cout << "P(int, int) " << a << " " << b << std::endl;
    }
    P(std::initializer_list<int> vars) {
      std::cout << "P(initializer_list )";
      for(auto a : vars)
        std::cout << a << " ";
      std::cout << std::endl;
    }
};

void print(std::initializer_list<int> vals) {
  for(auto iter = vals.begin(); iter != vals.end(); ++iter)
    std::cout << *iter << std::endl;
}

int main(int argc, char *argv[]) {
  int values[]{1, 2, 3};
  std::vector<int> v{2, 3, 5, 7, 11, 13, 17};
  std::vector<std::string> cities{
    "Beijing", "Nanjing", "Shanghai", "Hangzhou"
  };
  std::complex<double> c{3.0, 4.0};

  print({1, 2, 3, 4, 5});

  P p1(1, 2);
  P p2{1, 2};
  P p3{1, 2, 3};
  P p4 = {1, 2};
  P p5{};

  return 0;
}
