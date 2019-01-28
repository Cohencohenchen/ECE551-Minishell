#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>

//this program prints all the arguments it gets.
int main(int argc, char * argv[]) {
  for (int i = 1; i < argc; i++)
    std::cout << "Argument " << i << " is "
              << "\"" << argv[i] << "\"" << std::endl;
  return 0;
}
