#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define FORK_ERR_LEN       22
#define PIPE_ERR_LEN       31
#define CMD_ARGS_ERR_LEN   34
#define WRITE_PIPE_ERR_LEN 34
#define READ_PIPE_ERR_LEN  38


int
main(int argc, char *argv[]) {
    if (argc != 2) {
        write(2, "Error: wrong number of arguments!\n", CMD_ARGS_ERR_LEN);
        exit(1);
    }

    int pd_fst[2]; 
    int pd_snd[2];  
    if (pipe(pd_fst) < 0 || pipe(pd_snd)) {
        write(2, "Error: failed to create pipes!\n", PIPE_ERR_LEN);
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        write(2, "Error: failed to fork!\n", FORK_ERR_LEN);
        exit(2);
    }
    else if (pid == 0) {           // child proc
        close(pd_fst[1]);
        close(pd_snd[0]);

        int read_result;
        char buf;

        // read symbols from parent proc
        while ((read_result = read(pd_fst[0], &buf, 1)) > 0) {
            printf("%d: received '%c'\n", getpid(), buf); 

            // send symbol to parent proc
            if (write(pd_snd[1], &buf, 1) < 0) {
                write(2, "Error: failed to write into pipe!\n", WRITE_PIPE_ERR_LEN);
                close(pd_snd[1]);
                close(pd_fst[0]);
                exit(3);
            }            

        }
        if (read_result < 0) {
            write(2, "Error: failed to read data from pipe!\n", READ_PIPE_ERR_LEN);
            close(pd_fst[0]);
            close(pd_snd[1]);
            exit(4);
        }

        close(pd_fst[0]);
        close(pd_snd[1]);
        exit(0);
    }
    else {                         // parent proc  
        close(pd_fst[0]);
        close(pd_snd[1]);

        // send argument to child proc
        if (write(pd_fst[1], argv[1], strlen(argv[1])) < 0) {
            write(2, "Error: failed to write into pipe!\n", WRITE_PIPE_ERR_LEN);
            close(pd_fst[1]);
            close(pd_snd[0]);
            exit(5);
        }
        close(pd_fst[1]);

        int read_result;
        char buf;

        // read symbols from child proc
        while ((read_result = read(pd_snd[0], &buf, 1)) > 0) {
            printf("%d: received '%c'\n", getpid(), buf);
        }
        if (read_result < 0) {
            write(2, "Error: failed to read data from pipe!\n", READ_PIPE_ERR_LEN);
            close(pd_fst[1]);
            close(pd_snd[0]);
            exit(6);
        }
        close(pd_snd[0]);

        wait((int *)0);
    }

    exit(0);
}