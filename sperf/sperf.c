#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DEBUG
#include "debug.h"

char arg0[8] = "strace";
char arg1[4] = "-T";

void sperf(int, char *[]);
void child(int, int, char *[]);
void parent(int);

int main(int argc, char *argv[]) {
  Assert(argc > 1, "Usage: sperf-32/64 cmd -arg1 -arg2 ...");
  sperf(argc, argv);
  return 0;
}

void sperf(int argc, char *argv[]) {
  int cpid = 0;
  int pipefd[2] = {};

  Assert(pipe(pipefd) != -1, "Pipe failed.");
  cpid = fork();
  Assert(cpid != -1, "Fork failed.");

  if (cpid == 0) {
    /* child process */
    close(pipefd[0]);
    child(pipefd[1], argc, argv);
    Panic("Should not return from child!");
  } else {
    /* parent process */
    parent(pipefd[0]);
    close(pipefd[0]);
    close(pipefd[1]);
    //wait(NULL);
  }
}

void child(int fd, int argc, char *argv[]) {
  char **real_argv = malloc((argc + 1) * sizeof(char *));
  real_argv[0] = arg0;
  real_argv[1] = arg1;
  memcpy(real_argv + 2, argv + 1, (argc - 1) * sizeof(char *));

  // not execve because we need environmental variables
  dup2(fd, 2); // stderr
  execvp(real_argv[0], real_argv); 
  Log(real_argv[0]);
  Panic("strace is not executable. (NO PATH HITS.)");
}

void parent(int fd) {
  char buf = 0;
  char line[1024] = "";
  int length = 0;
  while (read(fd, &buf, 1) != EOF) {
    line[length++] = buf;
    if (buf == '\n') {
      line[length] = 0;
      Log("%s, EOL", line);
      length = 0;
    }
  }
}
