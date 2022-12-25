
// fork.c: create a new process

#include "kernel/types.h"
#include "user/user.h"

int
main()
{
  int pid;

  printf("fork() starting\n");

  pid = fork();

  if(pid == 0){
    printf("child, pid: %d\n", pid);
  } else {
    wait(0);
    printf("parent, pid: %d\n", pid);
  }
  
  exit(0);
}
