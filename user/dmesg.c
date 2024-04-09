#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUFFSIZE 2 * 4096 + 1
#define DMESG_ERR_LEN 32

int
main() {
    char buffer[BUFFSIZE];
    int mesg_size = dmesg(buffer);
    if (mesg_size < 0) {
        write(2, "Error: failed to execute dmesg!\n", DMESG_ERR_LEN);
        exit(1);
    }
    

    // skip incompletely removed message
    char *head = buffer;
    for (int i = 0; i < BUFFSIZE - 1; i++) {
        if (buffer[i] == '\n') {
            head = &buffer[i + 1];
            break;
        }
    }

    printf("%s", head);
    exit(0);
}