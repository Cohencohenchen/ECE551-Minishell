#include "MyVar.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

extern char ** environ;
void MyVar::InitPath() {
  this->ECE551PATH.clear();
  char * pathlist = getenv("PATH");
  int index = 0;
  std::string dir;
  while (pathlist[index] != '\0') {
    if (pathlist[index] != ':') {
      dir = dir + pathlist[index];
    }
    else {
      this->ECE551PATH.push_back(dir);
      dir.clear();
    }
    index++;
  }
  this->ECE551PATH.push_back(dir);
}
//get PATH from env, split it at ':' and put it in the Path.

void MyVar::InitVar() {
  int num = 0;
  std::string name;
  std::string value;
  while (environ[num] != NULL) {
    int index = 0;
    while (environ[num][index] != '=') {
      name = name + environ[num][index];
      index++;
    }
    index++;
    while (environ[num][index] != '\0') {
      value = value + environ[num][index];
      index++;
    }
    this->Var.insert(std::pair<std::string, std::string>(name, value));
    name.clear();
    value.clear();
    num++;
  }
  this->NumOfVar = num;
}
//reads every char * in environ and splits it at '=', changes two parts into two strings
//and store them as pair in map.

MyVar::MyVar() {
  InitPath();
  InitVar();
}
//Constructor of MyVar will do both initializations.

bool MyVar::VarExist(const std::string & name) {
  std::map<std::string, std::string>::iterator it;
  it = this->Var.find(name);
  if (it == this->Var.end())
    return false;
  return true;
}

bool CheckCharValid(char c) {
  if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
    return true;
  return false;
}
bool CheckNameValid(const std::string & name) {
  for (size_t i = 0; i < name.size(); i++) {
    if (!CheckCharValid(name[i]))
      return false;
  }
  return true;
}
//check if a string name is valid, every char should be in '0~9', 'a~z', 'A~Z', '_'

int MyVar::SetVar(const std::string & name, const std::string & value) {
  if (!CheckNameValid(name))
    return -1;
  std::map<std::string, std::string>::iterator it = this->Var.find(name);
  if (it == this->Var.end())
    this->Var.insert(std::pair<std::string, std::string>(name, value));
  else
    it->second = value;
  return 0;
}
//if the name is invalid, return -1
//else find the value in the map, and return.

std::string MyVar::GetValue(const std::string & name) {
  std::map<std::string, std::string>::iterator it = this->Var.find(name);
  if (it != this->Var.end())
    return it->second;
  return "";
}
//if the name exists, return the value, otherwise return an empty string.
