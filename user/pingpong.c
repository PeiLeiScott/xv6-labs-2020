#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Read and Write end of a pipe
#define READ_END 0
#define WRITE_END 1

int 
main(int argc, char *argv[]) 
{
  if (argc > 1) {
    fprintf(2, "Usage: pingpong\n");
    exit(1);
  }

  // Create two pipes: p1 and p2
  // Child reads from p1, writes to p2
  // Parent reads from p2, writes to p1
  int p1[2], p2[2];

  pipe(p1);
  pipe(p2);

  char buf[1];

  if (fork() == 0) { // Child should read first
    close(p1[WRITE_END]);
    close(p2[READ_END]);
    if (read(p1[READ_END], buf, 1) == 1) {
      close(p1[READ_END]);
      printf("%d: received ping\n", getpid());
    }
    // Child sends a byte to parent
    write(p2[WRITE_END], buf, 1);
    close(p2[WRITE_END]);
  } else { // Parent should write first
    close(p1[READ_END]);
    close(p2[WRITE_END]);
    // Parent sends a byte to child
    write(p1[WRITE_END], buf, 1);
    close(p1[WRITE_END]);
    if (read(p2[READ_END], buf, 1) == 1) {
      close(p2[READ_END]);
      printf("%d: received pong\n", getpid());
    }
  }

  exit(0);
}