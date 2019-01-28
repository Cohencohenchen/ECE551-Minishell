#ifndef __MYVAR_H__
#define __MYVAR_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>
//************************************************************************************************
//
//MyVar class is used for storing the variables--either from
//
//environment or set by the user--and the environment path.
//
//************************************************************************************************
extern char ** environ;
class MyVar
{
 public:
  int NumOfVar;
  //the number of variables in the shell

  std::map<std::string, std::string> Var;
  //A map for storing the name and value of these variable.

  std::vector<std::string> ECE551PATH;
  //A vector of the environment path,
  //which will be initialized in constructor method of MyVar class
  //and be updated when PATH is exported.

 public:
  int SetVar(const std::string & name, const std::string & value);
  //Put a given pair of variable name and value into the map.

  std::string GetValue(const std::string & name);
  //For a given variable, return its value; if it doesn't exist, return a empty string.

  MyVar();
  //Constructor method in which Var and ECE551path will be initialized.

  void InitPath();
  //Get the value of environment path.

  void InitVar();
  //Get the value of environment variables.

  bool VarExist(const std::string & name);
  //Check if a variable exists.
};

bool CheckCharValid(char c);
bool CheckNameValid(std::string & name);
//Check if a variable name is valid--that is, consisting of '1~9', 'a~z', 'A~Z', and '_'.
#endif
