#ifndef __MYSHELL_H__
#define __MYSHELL_H__

#include <string.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Command.h"
#include "MyVar.h"
#include "StringMath.h"

class myShell
{
  MyVar var;
  //class MyVar stores variables and path

  Command command;
  //class Command stores command arguments and how to translate it.

 public:
  void ShellGetCommand();
  //get command line from input.

  void ShellTransCommand();
  //translate command.

  int ShellCommandType();
  //return the value of Command Type, such 'EXE', 'CD', etc.

  int SetMyvar();
  //store a given name and value pair in the map, if the name exists, overwrite it.
  //On failure return -1;

  void Execute();
  //execute command stored in command class.

  int Export();
  //export variable into the env, return -1 on failure.

  int ChangeDir();
  //change dir, return -1 on failure.

  void Inc();
  //increase the variable stored in command class by one.
};

#endif
