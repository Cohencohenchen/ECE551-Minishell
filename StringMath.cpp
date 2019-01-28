/**************************************************************************

StringMath is used to inplement inc command.

The core function StringAddone reads a string and increase it by one and outputs.

**************************************************************************/
#include "StringMath.h"

#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>

//Find the position of '.' in a string
//return the index of '.'
//if there is no '.', return the last index of the string, which is '\0'
size_t FindDot(const std::string & s) {
  size_t index = 0;
  while (index < s.size() && s[index] != '.')
    index++;
  return index;
}

//Find the last non-zero digit of the decimal part
size_t FindLastValidDecimalDigit(const std::string & s) {
  size_t dot = FindDot(s);
  size_t last = s.size();
  while ((s[last] > '9' || s[last] < '1') && last > dot)
    last--;
  return last;
}

//IsDigit checks if a char is a digit
//IsNumber checks if a string can represent a number (including integer and decimal
bool IsDigit(char c) {
  if (c >= '0' && c <= '9')
    return true;
  return false;
}

bool IsNumber(const std::string & s) {
  size_t dot = FindDot(s);
  for (size_t i = dot + 1; i < s.size(); i++) {
    if (!IsDigit(s[i]))
      return false;
  }
  for (size_t j = dot - 1; j > 0; j--) {
    if (!IsDigit(s[j]))
      return false;
  }
  if (IsDigit(s[0]))
    return true;
  else if (s[0] == '-' && dot != 1)
    return true;
  return false;
}

//convert a integer to a string
std::string IntToString(int n) {
  std::string sign, result;
  if (n < 0) {
    sign = "-";
    n = -n;
  }
  while (n > 0) {
    result = char(n % 10 + 48) + result;
    n /= 10;
  }
  if (result.empty())
    result = "0";
  return sign + result;
}

std::string StringAddOne(const std::string & OldNum) {
  size_t dot = FindDot(OldNum);
  size_t last = FindLastValidDecimalDigit(OldNum);
  std::string NewNum;
  int OldInteger = atoi(OldNum.c_str());
  if (OldInteger == -1 && last != dot) {  //deal with the cases like -1.2 + 1 = -0.2
    NewNum = "-0" + OldNum.substr(dot, -1);
  }
  else if (OldInteger == 0 && OldNum[0] == '-' &&
           last != dot) {  //deal with the cases like -0.2 + 1 = 0.8
    std::string temp = OldNum;
    temp[last] = char(53 - (temp[last] - '5'));
    for (size_t i = last - 1; i > dot; i--)
      temp[i] = char(52 - (temp[i] - '5'));
    NewNum = "0" + temp.substr(dot, -1);
  }
  else {  //deal with the normal cases
    OldInteger++;
    NewNum = IntToString(OldInteger);
    if (dot < OldNum.size())
      NewNum = NewNum + OldNum.substr(dot, -1);
  }
  return NewNum;
}
