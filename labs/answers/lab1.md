- [Lab: Xv6 and Unix utilities](#lab-xv6-and-unix-utilities)
- [sleep](#sleep)
- [pingpong](#pingpong)
- [primes](#primes)
- [find](#find)
- [xargs](#xargs)


# Lab: Xv6 and Unix utilities
https://pdos.csail.mit.edu/6.S081/2022/labs/util.html

# sleep
```c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int ticks;

  if (argc != 2) {
    fprintf(2, "Usage: sleep ${ticks}\n");
    exit(1);
  }

  ticks = atoi(argv[1]);
  if (sleep(ticks) < 0) {
    fprintf(2, "sleep: %d failed\n", ticks);
    exit(1);
  }

  exit(0);
}
```

# pingpong
```c
#include "kernel/types.h"
#include "user/user.h"

#define READ 0 
#define WRITE 1

int main(int argc, char const *argv[]) {
    char buf = 'P';

    int fd_c2p[2]; // child to parent
    int fd_p2c[2]; // parent to child
    pipe(fd_c2p);
    pipe(fd_p2c);

    int pid = fork();

    if (pid < 0) {
        fprintf(2, "fork() error!\n");
        close(fd_c2p[READ]);
        close(fd_c2p[WRITE]);
        close(fd_p2c[READ]);
        close(fd_p2c[WRITE]);
        exit(1);
    } 
    
    // child
    if (pid == 0) { 
        close(fd_p2c[WRITE]);
        close(fd_c2p[READ]);

        if (read(fd_p2c[READ], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "child read() error!\n");
            exit(1);
        } else {
            fprintf(1, "child %d: received ping\n", getpid());
        }

        if (write(fd_c2p[WRITE], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "child write() error!\n");
            exit(1);
        }

        close(fd_p2c[READ]);
        close(fd_c2p[WRITE]);

        exit(0);
    // parent    
    } else { 
        close(fd_p2c[READ]);
        close(fd_c2p[WRITE]);

        if (write(fd_p2c[WRITE], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "parent write() error!\n");
            exit(1);
        }

        if (read(fd_c2p[READ], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "parent read() error!\n");
            exit(1); 
        } else {
            fprintf(1, "parent %d: received pong\n", getpid());
        }

        close(fd_p2c[WRITE]);
        close(fd_c2p[READ]);

        exit(0);
    }
}
```

# primes
undone

# find
```c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int compare(char *path, char *name) {
  char *p;
  // Find first character after last slash
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  return strcmp(p, name);
}

void find(char *path, char *name) {
  char buf[512], *p;
  int fd;
  struct stat st;
  struct dirent de;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
  case T_FILE:
    if (compare(path, name) == 0) {
      printf("%s\n", path);
    }
    break;
  case T_DIR:
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
        continue;
      }
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      find(buf, name);
    }
    break;
  }

  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "Usage: find <directory> <name>\n");
    exit(1);
  }

  find(argv[1], argv[2]);

  exit(0);
}

```

# xargs
```c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAX_LINE_LEN 512

int readline(char *buf) {
  char *p;
  int n;

  p = buf;
  while ((n = read(0, p, 1)) != 0) {
    if (*p == '\n') {
      *p = 0;
      return 0;
    }
    p++;
    if (p >= buf + MAX_LINE_LEN) {
      fprintf(2, "line too long\n");
      exit(1);
    }
  }

  *p = 0;
  return 1;
}

int main(int argc, char *argv[]) {
  char line[MAX_LINE_LEN];
  int read_end;
  int pid;
  char *args[MAXARG];
  int i;

  for (i = 1; i < argc; i++) {
    args[i-1] = argv[i];
  }

  do {
    read_end = readline(line);
    if (line[0] == 0) {
      continue;
    }

    pid = fork();
    if (pid == 0) {
      args[argc-1] = line;
      exec(argv[1], args);
      fprintf(2, "exec: %s failed\n", argv[1]);
      exit(1);
    } else {
      wait(0);
    }
  } while (read_end != 1);

  exit(0);
}
```