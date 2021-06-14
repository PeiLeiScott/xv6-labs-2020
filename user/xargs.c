#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

int 
main(int argc, char *argv[]) 
{
  if (argc < 2) {
    fprintf(2, "Usage: xargs <command>\n");
    exit(1);
  }

  char *cmd = argv[1];
  char *params[MAXARG], buf[512];
  int i;

  if (argc + 1 > MAXARG) {
    fprintf(2, "Too many arguments\n");
    exit(1);
  }

  for (i = 1; i < argc; i++) {
    params[i-1] = argv[i];
  }
  // params[argc-1] is the command argument from standard input
  // params[argc] is the NULL termination
  params[argc] = 0;

  while (1) {
    i = 0;
    while (1) {
      int n = read(0, &buf[i], 1);
      // n == 0 means it reaches the end of the argument from standard input
      // buf[i] == '\n' means it reaches the end of current line
      if (n == 0 || buf[i] == '\n') 
        break;
      i++;
    }
    // i == 0 means there's nothing left to be read
    if (i == 0) 
      break;
    buf[i] = 0;
    params[argc-1] = buf;
    if (fork() == 0) {
      exec(cmd, params);
      exit(0);
    } else {
      // Wait for the child to complete the command
      wait((int *) 0);
    }
  }

  exit(0);
}