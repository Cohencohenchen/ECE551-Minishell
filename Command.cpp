#include "Command.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "MyVar.h"

void Command::GetCommandline() {
  int c;
  //initialize every time we get a new line
  this->Commandline.clear();
  this->CommandArgu.clear();
  this->StdinRedirFile.clear();
  this->StdoutRedirFile.clear();
  this->StderrRedirFile.clear();
  while ((c = fgetc(stdin)) != '\n') {  //when reads '\n', stops.
    if (c == EOF) {                     //when reads EOF, stops
      this->CommandType = EXIT;
      return;
    }
    this->Commandline = this->Commandline + char(c);  //else add to current line,
  }
}

bool Command::AddToArguListOrNot(bool & StdinRedir,
                                 bool & StdoutRedir,
                                 bool & StderrRedir,
                                 std::string & arg) {
  if (StdinRedir) {                      //input redirection flag is true
    if (this->StdinRedirFile.empty()) {  //if the input redirection filename is empty,
      this->StdinRedirFile = arg;        //arg will be put into it, and flag is restored.
      StdinRedir = false;
    }
    else {
      this->CommandType = REDIRERR;  //else we have more than one filename for one redirection.
    }                                //it is wrong
    return false;  //return false to tell this arg can't be put in the argument list.
  }
  else if (StdoutRedir) {
    if (this->StdoutRedirFile.empty()) {
      this->StdoutRedirFile = arg;
      StdoutRedir = false;
    }
    else {
      this->CommandType = REDIRERR;
    }
    return false;
  }
  else if (StderrRedir) {
    if (this->StderrRedirFile.empty()) {
      this->StderrRedirFile = arg;
      StderrRedir = false;
    }
    else {
      this->CommandType = REDIRERR;
    }
    return false;
  }
  else if (arg == "<" && !StdinRedir) {  //if we get a '<' and it's not in redirection mode,
    StdinRedir = true;                   //redirection mode is started.
    return false;                        //the '<' can't be put into the argument list/
  }
  else if (arg == ">" && !StdoutRedir) {
    StdoutRedir = true;
    return false;
  }
  else if (arg == "2>" && !StderrRedir) {
    StderrRedir = true;
    return false;
  }
  else
    return true;
}

void Command::TransCommand(MyVar & var) {
  bool Escape = false;      //flag for if we should escape.
  bool SkipSpace = true;    //flag for if we should skipspaces
  bool StdinRedir = false;  //flags for redirection.
  bool StdoutRedir = false;
  bool StderrRedir = false;
  size_t
      next_index;  //we will skim the input line from start to end, next_index tells the next index.
  std::string arg, name;  //arg is the argument we just cut off from the line.
  //name is used for $, replace the name with its value from var map.
  for (size_t i = 0; i < this->Commandline.size();) {
    if (Escape == true) {  //if the escape flag is true, we get whatever shell reads, and reset
      arg = arg + this->Commandline[i];  //the escape flag.
      Escape = false;
      next_index = i + 1;
    }
    else {
      if (this->Commandline[i] == '\\') {  //if it's not in escape state and we get a '\',
        Escape = true;                     //we get into the escape state and omits this '\'.
        next_index = i + 1;
      }
      else if (this->Commandline[i] == ' ') {  //if we get a ' ' when not on escape state.
        if (SkipSpace == false && this->CommandArgu.size() >= 2 &&
            !arg.empty()) {  //if we are in skipspace state and we are in the process of dealing with
          arg = arg + ' ';   //the third argument, ' ' will be put in argument.
        }
        else if (
            (SkipSpace == true || (SkipSpace == false && this->CommandArgu.size() < 2)) &&
            !arg.empty()) {  //the condition we can get a new argument, the first part tests if this ' ' can be spilt and the second part tests if it should be split
          if (this->CommandArgu.empty() == true && arg == "set")
            SkipSpace = false;
          //For Redirection, we will consider where to put these arguments.
          if (AddToArguListOrNot(StdinRedir, StdoutRedir, StderrRedir, arg) == true)
            this->CommandArgu.push_back(arg);
          arg.clear();
        }
        next_index = i + 1;
      }
      else if (this->Commandline[i] == '$') {  //if we get a '$'
        name.clear();                          //a new mapping will start, clear the old name.
        size_t j = i + 1;
        while (CheckCharValid(this->Commandline[j]) == true &&
               j < this->Commandline.size()) {  //if the input char is valid, put it in the name.
          name = name + this->Commandline[j];
          j++;
        }
        if (name.empty() ==
            false)  //if the newly got name isn't empty, look it up in the map, and put the value in the argument.
          arg = arg + var.GetValue(name);
        next_index = j;
      }
      else {  //if all the above doesn't apply
        arg = arg + this->Commandline[i];
        next_index = i + 1;
      }
    }
    i = next_index;
  }
  if (arg.empty() ==
      false) {  //if the arguement is at the end of commandline, it should be added to the argulist.
    if (AddToArguListOrNot(StdinRedir, StdoutRedir, StderrRedir, arg) == true)
      this->CommandArgu.push_back(arg);
    arg.clear();
  }
  if (this->CommandArgu.empty() == true)  //dicide the command type according to the argument list.
    this->CommandType = IDLE;
  else if (this->CommandArgu[0] == "set")
    this->CommandType = SET;
  else if (this->CommandArgu[0] == "export")
    this->CommandType = EXPO;
  else if (this->CommandArgu[0] == "cd")
    this->CommandType = CD;
  else if (this->CommandArgu[0] == "inc")
    this->CommandType = INC;
  else if (this->CommandArgu[0] == "exit")
    this->CommandType = EXIT;
  else {
    if ((StdinRedir) || (StdoutRedir) || (StderrRedir)) {
      this->CommandType = REDIRERR;
    }
    else
      this->CommandType = EXE;
  }
}

char ** StringsToChars(const std::vector<std::string> & strings, int start) {
  int Num = strings.size();
  if (start > Num - 1)
    return NULL;
  char ** Chars = new char *[Num + 1 - start];
  for (int i = start; i < Num; i++) {
    int len = strings[i].size();
    Chars[i - start] = new char[len + 1];
    for (int j = 0; j < len; j++)
      Chars[i - start][j] = strings[i][j];
    Chars[i - start][len] = '\0';
  }
  Chars[Num - start] = NULL;
  return Chars;
}
//change a vector of strings to a char * array, from the given index.

void DeleteChars(char ** Chars) {
  int i = 0;
  while (Chars[i] != NULL) {
    delete[] Chars[i];
    i++;
  }
  delete[] Chars;
}
//delete the newed memory.

int FindPathCato(const std::string & filename) {
  size_t index = 0;
  while (filename[index] != '/' && filename[index] != '\0')
    index++;
  if (filename[index] == '\0')
    return ENVPATH;
  else {
    if (filename[0] == '/')
      return ABSPATH;
    else
      return RELAPATH;
  }
}
//dicide if a command is in abspath, relapath or we need to find path for it in env.

Command::Command() : Commandline(), CommandArgu(), CommandType(0) {}
Command::~Command() {}
