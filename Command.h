#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "MyVar.h"

#define EXIT -1
#define CD 1
#define INC 2
#define EXE 3
#define SET 4
#define EXPO 5
#define IDLE 6

#define RELAPATH 7
#define ABSPATH 8
#define ENVPATH 9
#define REDIRERR 10
//************************************************************************************************
//
//class Command gets input from stdin, translates it into
//
//command types and arguments for further function.
//
//************************************************************************************************

class Command
{
 public:
  std::string Commandline;
  //A string to store the input from stdin.

  std::vector<std::string> CommandArgu;
  //A vector of strings to store the parsed commands, including the command name and its arguments,
  //if any.

  int CommandType;
  //The catogory of the command, including:
  //exit(EXIT);
  //change directory(CD);
  //increase the number by one(INC);
  //set value(SET);
  //export variable to environment(EXPO);
  //execute a program(EXE);
  //do nothing(IDLE).

  std::string StdinRedirFile;
  std::string StdoutRedirFile;
  std::string StderrRedirFile;

 public:
  Command();
  ~Command();
  void GetCommandline();
  //get input into CommandLine.

  void TransCommand(MyVar & var);
  //Parse the CommandLine with the help of var map from MyVar class.

  bool AddToArguListOrNot(bool & StdinRedir,
                          bool & StdoutRedir,
                          bool & StderrRedir,
                          std::string & arg);
};
//to check if the newly parsed argument should be put in the commandArgu list or should
//be put in the Std redirection strings.

char ** StringsToChars(const std::vector<std::string> & strings, int start);
//get a char * array from a vector of strings, starting at the given index

void DeleteChars(char ** Chars);
//delete the char * array newed by StringsTochars.

int FindPathCato(const std::string & filename);
//Given a path, return the catogory of the path: whether it is a abssolute path, a relative path,
//or we need to find it in the environment path.

#endif
