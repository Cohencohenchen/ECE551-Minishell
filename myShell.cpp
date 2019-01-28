#include "myShell.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Command.h"
#include "MyVar.h"
#include "StringMath.h"

int myShell::SetMyvar() {
  if (this->command.CommandArgu.size() != 3) {
    std::cout << "Wrong input format for set command" << std::endl;
    return -1;
  }
  //check the number of arguments is 3

  else if (this->var.SetVar(this->command.CommandArgu[1], this->command.CommandArgu[2]) == -1) {
    std::cout << "Invalid variable name" << std::endl;
    return -1;
  }
  //if the variable name can't be found in var map
  return 0;
}

int myShell::ChangeDir() {
  if (this->command.CommandArgu.size() != 2) {
    std::cout << "Wrong input format for cd command" << std::endl;
    return -1;
  }
  //check the input argument numbers

  struct stat sb;
  if (stat(this->command.CommandArgu[1].c_str(), &sb) == -1) {
    std::cout << "directory not exist" << std::endl;
    return -1;
  }
  if ((sb.st_mode & S_IFMT) != S_IFDIR) {
    std::cout << "not a directory" << std::endl;
    return -1;
  }
  //check what we are looking for exsits and is a directory.

  if (chdir(this->command.CommandArgu[1].c_str()) != 0) {
    std::cout << "cannot change directory" << std::endl;
    return -1;
  }
  //if chdir fails, return -1.

  char * pwdname = get_current_dir_name();
  this->var.SetVar("PWD", pwdname);
  setenv("PWD", pwdname, 1);
  //after cd command, we will need to update both the environment and the local var map.

  free(pwdname);
  return 0;
}

int myShell::Export() {
  if (this->command.CommandArgu.size() != 2) {
    std::cout << "Wrong input format for export command" << std::endl;
    return -1;
  }
  //check format

  else {
    if (this->var.VarExist(this->command.CommandArgu[1]) == false) {
      std::cout << "No such variable" << std::endl;
      return -1;
    }
    //check if variable exists

    else {
      setenv(this->command.CommandArgu[1].c_str(),
             this->var.GetValue(this->command.CommandArgu[1]).c_str(),
             1);
      if (this->command.CommandArgu[1] == "PATH")
        var.InitPath();
      //set the varibale in env, if what we changing is PATH, we need to update local PATH vector.

      return 0;
    }
  }
}

void myShell::Inc() {
  if (this->command.CommandArgu.size() != 2)
    std::cout << "Wrong input format for inc command" << std::endl;
  //check format

  else {
    if (this->var.VarExist(this->command.CommandArgu[1])) {
      std::string value = this->var.GetValue(this->command.CommandArgu[1]);
      if (IsNumber(value)) {
        this->var.SetVar(this->command.CommandArgu[1], StringAddOne(value));
        return;
      }
      //if the value is a number, add it by one.
    }
    this->var.SetVar(this->command.CommandArgu[1], "1");
  }
  //else, set it to 1.
}

void myShell::Execute() {
  int PathCato =
      FindPathCato(this->command.CommandArgu[0]);  //find the type of path of the execute file
  size_t index = 0;
  if (PathCato == ABSPATH ||
      PathCato == RELAPATH) {  //if it is abspath or relative path, no need to search the envpath
    if (access(this->command.CommandArgu[0].c_str(), F_OK) != 0) {  //not exist
      std::cout << "Command " << this->command.CommandArgu[0] << " not found" << std::endl;
      return;
    }
    else if (access(this->command.CommandArgu[0].c_str(), X_OK) != 0) {  //exist but not executable
      std::cout << "Command " << this->command.CommandArgu[0] << " not executable" << std::endl;
      return;
    }
    struct stat sb;
    if (stat(this->command.CommandArgu[0].c_str(), &sb) != -1 && (sb.st_mode & S_IFMT) == S_IFDIR) {
      std::cout << this->command.CommandArgu[0] << " is a directory" << std::endl;
      return;
    }
  }
  else {  //search the envpath for the execute file
    while (index != this->var.ECE551PATH.size() &&
           access((this->var.ECE551PATH[index] + "/" + this->command.CommandArgu[0]).c_str(),
                  F_OK) != 0)
      index++;
    if (index == this->var.ECE551PATH.size()) {  //not found
      std::cout << "Command " << this->command.CommandArgu[0] << " not found" << std::endl;
      return;
    }
    else if (access((this->var.ECE551PATH[index] + "/" + this->command.CommandArgu[0]).c_str(),
                    X_OK) != 0 ||
             this->command.CommandArgu[0] == "." ||
             this->command.CommandArgu[0] == "..") {  //found but not excutable
      //for some reason, access("usr/local/sbin/.", X_OK) == 0 , access("usr/local/sbin/..", X_OK) ==0.
      std::cout << "Command " << this->command.CommandArgu[0] << " not executable" << std::endl;
      return;
    }
  }
  pid_t cpid, wpid;
  int status;
  char ** ExeArgu = StringsToChars(this->command.CommandArgu, 0);
  cpid = fork();
  if (cpid == -1) {
    perror("fork error");
    exit(EXIT_FAILURE);
  }
  else if (cpid == 0) {                           //for child process
    if (!this->command.StdinRedirFile.empty()) {  //check if we need to redirect input
      int oldfd = open(this->command.StdinRedirFile.c_str(), O_RDONLY);
      if (oldfd == -1) {
        perror("Open Failed");
        exit(EXIT_FAILURE);
      }
      dup2(oldfd, 0);
      close(oldfd);
    }
    if (!this->command.StdoutRedirFile.empty()) {  //check if we need to redirect output
      int oldfd = open(this->command.StdoutRedirFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
      if (oldfd == -1) {
        perror("Open Failed");
        exit(EXIT_FAILURE);
      }
      dup2(oldfd, 1);
      close(oldfd);
    }
    if (!this->command.StderrRedirFile.empty()) {  //check if we need to redirect stderror
      int oldfd = open(this->command.StderrRedirFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
      if (oldfd == -1) {
        perror("Open Failed");
        exit(EXIT_FAILURE);
      }
      dup2(oldfd, 2);
      close(oldfd);
    }  //finish redirection.

    if (PathCato == ABSPATH || PathCato == RELAPATH) {
      // char ** ExeArgu = StringsToChars(this->command.CommandArgu, 0);
      execve(this->command.CommandArgu[0].c_str(), ExeArgu, environ);
      // DeleteChars(ExeArgu);
    }
    else {
      //  char ** ExeArgu = StringsToChars(this->command.CommandArgu, 0);
      execve((this->var.ECE551PATH[index] + "/" + this->command.CommandArgu[0]).c_str(),
             ExeArgu,
             environ);
      // DeleteChars(ExeArgu);
    }  //execute the commands.
  }
  else {  //for parent process
    wpid = waitpid(cpid, &status, WUNTRACED | WCONTINUED);
    if (wpid == -1) {
      perror("waitpid error");
      exit(EXIT_FAILURE);
    }
    if (WIFEXITED(status))  //test the status of the child process after it exits.
      std::cout << "Programd exited with status " << WEXITSTATUS(status) << std::endl;
    else if (WIFSIGNALED(status))
      std::cout << "Program killed by signal " << WTERMSIG(status) << std::endl;
    DeleteChars(ExeArgu);
  }
}

void myShell::ShellGetCommand() {
  this->command.GetCommandline();
}

void myShell::ShellTransCommand() {
  this->command.TransCommand(this->var);
}
int myShell::ShellCommandType() {
  return this->command.CommandType;
}
//get, parse, and label the command input.

int main() {
  myShell shell;
  while (true) {
    char * dirname = get_current_dir_name();
    std::cout << "myShell:" << dirname << " $ ";
    free(dirname);
    //output the prompt.

    shell.ShellGetCommand();
    int ComType = shell.ShellCommandType();
    if (ComType == EXIT) {
      std::cout << std::endl;  //in case of EOF, ComType is set to be EXIT
      return EXIT_SUCCESS;     //exit directly.
    }
    shell.ShellTransCommand();           //else, parse the command line input.
    ComType = shell.ShellCommandType();  //get the command type.
    if (ComType == EXIT)
      return EXIT_SUCCESS;
    else if (ComType == IDLE)
      continue;
    else if (ComType == CD) {
      shell.ChangeDir();
    }
    else if (ComType == SET) {
      shell.SetMyvar();
    }
    else if (ComType == EXPO) {
      shell.Export();
    }
    else if (ComType == INC) {
      shell.Inc();
    }
    else if (
        ComType ==
        REDIRERR) {  //redirection input format is wrong, such as no filenames after '>'/'<'/'2>' or multiple filenames for one redirection.
      perror("rediretion format error\n");
    }
    else {
      shell.Execute();  //execute program.
    }
  }
  return EXIT_SUCCESS;
}
