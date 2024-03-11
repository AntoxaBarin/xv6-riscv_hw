#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 128 - max length of command in xv6
#define BUF_LEN 128

#define FORK_ERR_LEN 24
#define READ_ERR_LEN 39
#define PIPE_ERR_LEN 30
#define WRT_PIPE_ERR_LEN 39

int 
main(int argc, char* argv[]) { 
    int p[2];               // pipe's file descriptors
    
    if (pipe(p) < 0) {
        write(2, "Error: failed to create pipe!\n", PIPE_ERR_LEN);
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {          // fork error
        write(2, "Error: failed to fork!\n", FORK_ERR_LEN); 
        exit(1);
    }
    else if (pid == 0) {    // child process
        char buf[BUF_LEN];
        int read_rslt;

        close(p[1]);        // p[1]  --x--> pipe_write
        while ((read_rslt = read(p[0], buf, BUF_LEN)) > 0) {
            write(1, buf, read_rslt); 
        }
        if (read_rslt < 0) {
            write(2, "Error: failed to read data from pipe!\n", READ_ERR_LEN);
            exit(1);
        }

        close(p[0]);        // p[0]  --x--> pipe_read
        exit(0);
    } 
    else {                  // parent process
        close(p[0]);        // p[0]  --x--> pipe_read
        for (int i = 1; i < argc; i++) {
            if (write(p[1], argv[i], strlen(argv[i])) < 0) {
                write(2, "Error: failed to write data into pipe!\n", WRT_PIPE_ERR_LEN);
                close(p[1]); 
                exit(1);
            }
            if (write(p[1], "\n", 1) < 0) {
                write(2, "Error: failed to write data into pipe!\n", WRT_PIPE_ERR_LEN);
                close(p[1]);   
                exit(1);
            }
        }
        close(p[1]);        // p[1]  --x--> pipe_write
        
        wait((int *) 0);
    }

    exit(0);
}
