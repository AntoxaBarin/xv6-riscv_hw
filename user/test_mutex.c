#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define FORK_ERR_LEN        22
#define PIPE_ERR_LEN        31
#define CMD_ARGS_ERR_LEN    34
#define WRITE_PIPE_ERR_LEN  34
#define READ_PIPE_ERR_LEN   38
#define MTX_CREATE_ERR_LEN  31
#define MTX_LOCK_ERR_LEN    33
#define MTX_UNLOCK_ERR_LEN  35
#define MTX_DESTROY_ERR_LEN 32

int
main(int argc, char *argv[]) {
    if (argc != 2) {
        write(2, "Error: wrong number of arguments!\n", CMD_ARGS_ERR_LEN);
        exit(3);
    }
    
    // create mutex
    int mutex = mutex_create();
    if (mutex < 0) {
        write(2, "Error: failed to create mutex!\n", MTX_CREATE_ERR_LEN);
        exit(6);
    }

    int pd_fst[2]; 
    int pd_snd[2];  
    if (pipe(pd_fst) < 0 || pipe(pd_snd)) {
        write(2, "Error: failed to create pipes!\n", PIPE_ERR_LEN);
        exit(2);
    }
    
    int pid = fork();
    if (pid < 0) {
        write(2, "Error: failed to fork!\n", FORK_ERR_LEN);
        exit(1);
    }
    else if (pid == 0) {           // child proc
        close(pd_fst[1]);
        close(pd_snd[0]);

        int read_result, mtx_code;
        char buf;

        // read symbols from parent proc
        while ((read_result = read(pd_fst[0], &buf, 1)) > 0) {
            
            mtx_code = mutex_lock(mutex);
            if (mtx_code < 0) {
                write(2, "Error: failed to lock the mutex!\n", MTX_LOCK_ERR_LEN);
                exit(7);
            }

            // mutex provides locking of console output        
            printf("%d: received '%c'\n", getpid(), buf); 
            
            mtx_code = mutex_unlock(mutex);
            if (mtx_code < 0) {
                write(2, "Error: failed to unlock the mutex!\n", MTX_UNLOCK_ERR_LEN);
                exit(8);
            }
            
            // send symbol to parent proc
            if (write(pd_snd[1], &buf, 1) < 0) {
                write(2, "Error: failed to write into pipe!\n", WRITE_PIPE_ERR_LEN);
                close(pd_snd[1]);
                close(pd_fst[0]);
                exit(4);
            }            
        }
        if (read_result < 0) {
            write(2, "Error: failed to read data from pipe!\n", READ_PIPE_ERR_LEN);
            close(pd_fst[0]);
            close(pd_snd[1]);
            exit(5);
        }

        close(pd_snd[1]);
        close(pd_fst[0]);
        
        if (mutex_destroy(mutex) < 0) {
            write(2, "Error: failed to destroy mutex!\n", MTX_DESTROY_ERR_LEN);
            exit(9);
        }
        
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
            exit(4);
        }
        close(pd_fst[1]);

        int read_result, mtx_code;
        char buf;

        // read symbols from child proc
        while ((read_result = read(pd_snd[0], &buf, 1)) > 0) {
            mtx_code = mutex_lock(mutex);
            if (mtx_code < 0) {
                write(2, "Error: failed to lock the mutex!\n", MTX_LOCK_ERR_LEN);
                exit(7);
            }
            // mutex provides locking of console output 
            printf("%d: received '%c'\n", getpid(), buf);

            mtx_code = mutex_unlock(mutex);
            if (mtx_code < 0) {
                write(2, "Error: failed to unlock the mutex!\n", MTX_UNLOCK_ERR_LEN);
                exit(8);
            }
        }
        if (read_result < 0) {
            write(2, "Error: failed to read data from pipe!\n", READ_PIPE_ERR_LEN);
            close(pd_fst[1]);
            close(pd_snd[0]);
            exit(5);
        }
        close(pd_snd[0]);

        if (mutex_destroy(mutex) < 0) {
            write(2, "Error: failed to destroy mutex!\n", MTX_DESTROY_ERR_LEN);
            exit(9);
        }

        wait((int *)0);
    }

    exit(0);
}