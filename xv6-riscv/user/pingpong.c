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