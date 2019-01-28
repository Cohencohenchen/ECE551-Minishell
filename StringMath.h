#ifndef __STRINGMATH_H__
#define __STRINGMATH_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>

//************************************************************************************************
//
//StringMath file is used to do string math--add an existing number string by one or set a non-number
//
//string to one.
//
//************************************************************************************************

bool IsDigit(char c);
bool IsNumber(const std::string & s);
//Check if a string is a valid decimal number

std::string IntToString(int n);
//change an integer number to a string

std::string StringAddOne(const std::string & OldNum);
//core function that reads a string and adds it by one

size_t FindDot(const std::string & s);
//find the position of '.' in a valid number string.

size_t FindLastValidDecimalDigit(const std::string & s);
//find the last non-zero digit in the decimal part, if there is no decimal part,
//it will point at the '.' or '\0'.

#endif
