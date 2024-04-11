#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NOT_ENGH_ARGS_ERR_LEN 35
#define TOO_MANY_ARGS_ERR_LEN 33
#define INC_TURN_ON_VALUE_ERR_LEN 83
#define INC_TICKS_NUM_ERR_LEN 47

#define syscall_event "1"
#define interrupt_event "2"
#define proc_switch_event "3"
#define exec_event "4"

#define turn_on_event "1"
#define turn_off_event "0"

int
main(int argc, char *argv[]) {
    if (argc < 3) {
        write(2, "Not enough command line arguments!\n", NOT_ENGH_ARGS_ERR_LEN);
        exit(1);
    }
    else if (argc > 4) {
        write(2, "Too many command line arguments!\n", TOO_MANY_ARGS_ERR_LEN);
        exit(1);
    }

    int turn_on = 0;
    if (!strcmp(argv[2], turn_on_event)) {
        turn_on = 1;
    }
    else if (strcmp(argv[2], turn_off_event)) {
        write(2, "Incorrect value of argument 2!\nType 1 to turn on / 0 to turn off class protocoling\n", INC_TURN_ON_VALUE_ERR_LEN);
        exit(1);
    }

    int ticks = 0;
    if (argc == 4) {
        ticks = atoi(argv[3]);

        if (ticks <= 0) {
            write(2, "Incorrect value of ticks number!\nUse 1 or more\n", INC_TICKS_NUM_ERR_LEN);
            exit(1);
        }
    }

    protmng(atoi(argv[1]), turn_on, ticks);
    exit(0);
}