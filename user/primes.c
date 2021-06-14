#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Read and Write end of a pipe
#define READ_END 0
#define WRITE_END 1

// pp, cp stands for parent pipe and child pipe, respectively
void
child(int pp[])
{
  close(pp[WRITE_END]);
  int i;
  if (read(pp[READ_END], &i, sizeof(i)) == 0) {
    close(pp[READ_END]);
    exit(0);
  }
  printf("prime %d\n", i);
  int num, cp[2];
  pipe(cp);
  if (fork() == 0) {
    close(pp[READ_END]);
    close(cp[WRITE_END]);
    child(cp);
  } else {
    close(cp[READ_END]);
    while (read(pp[READ_END], &num, sizeof(num)) != 0) {
      if (num % i != 0) {
        write(cp[WRITE_END], &num, sizeof(num));
      }
    }
    close(pp[READ_END]);
    close(cp[WRITE_END]);
    wait((int *) 0);
  }
  
  exit(0);
}

int 
main(int argc, char *argv[]) 
{
  if (argc > 1) {
    fprintf(2, "Usage: primes\n");
    exit(1);
  }

  int p[2];
  pipe(p);

  if (fork() == 0) {
    child(p);
  } else {
    close(p[READ_END]);
    // Feed the numbers 2 through 35 into the pipe
    for (int i = 2; i <= 35; i++) {
      write(p[WRITE_END], &i, sizeof(i));
    }
    close(p[WRITE_END]);
    wait((int *) 0);
  }

  exit(0);
}