#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUFFSIZE 2 * 4096 + 1
#define DMESG_ERR_LEN 32

int
main() {
    char *buffer = malloc(BUFFSIZE); 
    int mesg_size = dmesg(buffer);
    if (mesg_size < 0) {
        printf("%d\n", mesg_size);
        write(2, "Error: failed to execute dmesg!\n", DMESG_ERR_LEN);
        exit(1);
    }
    
    
    // skip incompletely removed message
    int head = 0;
    for (int i = 0; i < BUFFSIZE - 1; i++) {
        if (buffer[i] == '\n') {
            head = i + 1;
            break;
        }
    }
    
    char *head_ptr = &buffer[head];
    printf("%s", head_ptr);
    exit(0);
}