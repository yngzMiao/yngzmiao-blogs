#include "utils.h"
#include <iostream>

int main(int argc, char const *argv[])
{
  std::cout << "version is " << VERSION_MAJOR << ", " << VERSION_MINOR << ", "<< VERSION_PATCH << std::endl;
  std::cout << "timestamp is " << BUILD_TIMESTAMP << std::endl;
  std::cout << "git is " << GIT_BRANCH << ", " << GIT_HASH << std::endl;
  
  return 0;
}
