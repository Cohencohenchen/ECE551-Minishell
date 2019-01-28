#include <stdio.h>
#include <stdlib.h>

#include <iostream>

int main(int argc, char * argv[]) {
  int n = atoi(argv[1]);
  if (n <= 0) {
    perror("N must be positive");
    exit(EXIT_FAILURE);
  }
  std::string s;
  while (std::cin >> s) {
    for (int i = 0; i < n; i++)
      std::cout << s << std::endl;
  }
  return 0;
}
