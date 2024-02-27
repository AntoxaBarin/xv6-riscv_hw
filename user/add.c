#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 2 10-digit numbers, space symbol, '\n', '\0'
#define BUF_LEN 23

// lengths of error messages
#define INC_FST_ERR_LEN 24
#define INC_SND_ERR_LEN 25
#define INC_INPUT_ERR_LEN 17
#define WR_ARGS_NUM_ERR_LEN 27
#define BUF_OVFLW_ERR_LEN 17

int
main() {
    char buf[BUF_LEN] = "";      
    gets(buf, BUF_LEN);

    char *number_1;
    char *number_2;

    // pointer to symbol in buffer
    int i = 0;

    if ((buf[0] == '0' && buf[1] != ' ') || (buf[0] == ' ')) {
        write(2, "Incorrect first number!\n", INC_FST_ERR_LEN);
        exit(1);
    }
    number_1 = buf + i;

    // parse first number           
    while (buf[i] != ' ') {                         
        if (buf[i] < '0' || buf[i] > '9') {
            write(2, "Incorrect input!\n", INC_INPUT_ERR_LEN);
            exit(1);
        }
    
        if (i == (BUF_LEN - 1) && buf[i] != '\0') {
            write(2, "Wrong number of arguments!\n", WR_ARGS_NUM_ERR_LEN);
            exit(1);
        }   
        i++;
    }
    if ((i == 0) || (i == 1 && (*number_1 < '0' || *number_1 > '9'))) {
        write(2, "Wrong number of arguments!\n", WR_ARGS_NUM_ERR_LEN);
        exit(1);
    }   
   
    i++;       // skip space symbol between numbers

    if (buf[i] == '0' && !(buf[i + 1] == '\n' || buf[i + 1] == '\0')) {
        write(2, "Incorrect second number!\n", INC_SND_ERR_LEN);
        exit(1);
    } 
    number_2 = buf + i;
    
    // parse second number
    while (buf[i] != '\0') {                        
        if ((buf[i] < '0' || buf[i] > '9') && buf[i] != '\n') {
            write(2, "Incorrect input!\n", INC_INPUT_ERR_LEN);
            exit(1);
        }

        if (i == (BUF_LEN - 1) && buf[i] != '\0') {
            write(2, "Buffer overflow!\n", BUF_OVFLW_ERR_LEN);
            exit(1);
        }
        i++;
    }
    if ((buf + i - number_2 == 0) || (buf + i - number_2 == 1 && (*number_2 < '0' || *number_2 > '9'))) {
        write(2, "Wrong number of arguments!\n", WR_ARGS_NUM_ERR_LEN);
        exit(1);
    }
    

    int int_number1 = atoi(number_1);
    int int_number2 = atoi(number_2);

    // do system call
    printf("%d\n", add(int_number1, int_number2));
    
    exit(0);
}