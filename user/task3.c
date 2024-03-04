#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 128 - max length of command in xv6
#define BUF_LEN 128

#define FORK_ERR_LEN 24
#define EXEC_ERR_LEN 30

int
main(int argc, char* argv[]) {
    int p[2];
    pipe(p);
    
    int pid = fork();
    if (pid < 0) { 
        write(2, "Error: failed to fork!\n", FORK_ERR_LEN);
        exit(1);
    }
    else if (pid == 0) {       // child process
        char* args[2] = {"/wc", 0}; 
        
        close(0);              // 0    --x--> stdin             
        dup(p[0]);             // 0    -----> pipe_read
        close(p[0]);           // p[0] --x--> pipe_read
        close(p[1]);           // p[1] --x--> pipe_write
        
        exec("/wc", args);

        // This code unreachable if exec() succeed
        write(2, "Error: failed to execute wc!\n", EXEC_ERR_LEN);
	    exit(1);
    } 
    else {                      // parent process
        close(p[0]);            // p[0] --x--> pipe_read
        for (int i = 1; i < argc; i++) {
            write(p[1], argv[i], strlen(argv[i]));
            write(p[1], "\n", 1);
        } 
        
        close(p[1]);            // p[1] --x--> pipe_write
        wait((int *) 0);
    }

    exit(0);
}