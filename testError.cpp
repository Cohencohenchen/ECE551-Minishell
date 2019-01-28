#include <stdio.h>
#include <stdlib.h>

//This program only perrors an error message.
#include <iostream>
int main() {
  perror("This is a program that will exit(EXIT_FAILURE).");
  exit(EXIT_FAILURE);
}
