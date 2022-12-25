
// open.c: create a file, write to it.

#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int
main()
{
  int fd = open("out", O_WRONLY | O_CREATE | O_TRUNC);
  write(fd, "ooo\n", 4);

  exit(0);
}
