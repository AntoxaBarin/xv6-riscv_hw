#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define ARGS_ERR_LEN 27 
#define INC_FILE_SIZE_ERR_LEN 22
#define BAD_ALLOC_ERR_LEN 28
#define FILE_CREATE_ERR_LEN 24
#define BAD_WRITE_ERR_LEN 31
#define FILE_OPEN_ERR_LEN 22
#define FILE_READ_ERR_LEN 32
#define FILE_DATA_ERR_LEN 18

#define MULT 46459065878232913
#define INC 321123
#define MOD 9223372036854775783
#define BUF_SIZE 1024
#define MAX_FILE_SIZE (268 + 256 * 256) * 1024 // in bytes


#define file_name "file_name"

uint64 
generate_next(uint64 prev) {
    return (prev * MULT + INC) % MOD;
}

int
main(int argc, char* argv[]) {   
    if (argc != 3) {
        write(2, "Incorrect number of args!\n", ARGS_ERR_LEN);
        exit(1);
    }

    // Extracting command line args
    int file_size = atoi(argv[1]) / sizeof(uint64); // in 8-byte numbers
    int seed = atoi(argv[2]);
    uint64 cur_number = seed;   

    if (file_size <= 0 || file_size > MAX_FILE_SIZE) {
        write(2, "Incorrect file size!\n", INC_FILE_SIZE_ERR_LEN);
        exit(2);
    }
    uint64 *buffer = malloc(sizeof(uint64) * BUF_SIZE);
    if (buffer == 0) {
        write(2, "Failed to allocate buffer!\n", BAD_ALLOC_ERR_LEN);
        exit(3);
    }

    // Creating file 
    int fd_write = open(file_name, O_CREATE | O_WRONLY);
    if (fd_write < 0) {
        write(2, "Failed to create file!\n", FILE_CREATE_ERR_LEN);
        exit(4);
    }

    // Writing generated numbers to file
    for (int i = 0; i < file_size; ++i) {
        buffer[i % BUF_SIZE] = cur_number;

        if (i + 1 == file_size || (i + 1) % BUF_SIZE == 0) {
            int written_data_size = file_size % BUF_SIZE;
            if ((i + 1) % BUF_SIZE == 0) {
                written_data_size = BUF_SIZE;
            }
            // Failed to write numbers
            if (write(fd_write, buffer, sizeof(uint64) * written_data_size) != sizeof(uint64) * written_data_size) {
                close(fd_write);
                write(2, "Failed to write data to file!\n", BAD_WRITE_ERR_LEN);
                exit(5);
            }
        }
        cur_number = generate_next(cur_number);
    }
    close(fd_write);

    // Open file to read data
    int fd_read = open(file_name, O_RDONLY);
    if (fd_read < 0) {
        write(2, "Failed to open file!\n", FILE_OPEN_ERR_LEN);
        exit(4);
    }
    cur_number = seed;

    // Reading and comparing generated numbers from file
    for (int i = 0; i < file_size / BUF_SIZE; ++i) {
        if (read(fd_read, buffer, sizeof(uint64) * BUF_SIZE) != sizeof(uint64) * BUF_SIZE) {
            close(fd_read);
            write(2, "Failed to read data from file!\n", FILE_READ_ERR_LEN);
            exit(6);
        }
        for (int j = 0; j < BUF_SIZE; ++j) {
            if (cur_number != buffer[j]) {
                close(fd_read);
                write(2, "File data error!\n", FILE_DATA_ERR_LEN);
                exit(7);
            }
            cur_number = generate_next(cur_number);
        }
    }

    // Read tail of file
    int tail_size = file_size % BUF_SIZE;
    if (tail_size != 0) {
        if (read(fd_read, buffer, sizeof(uint64) * tail_size) != sizeof(uint64) * tail_size) {
            close(fd_read);
            write(2, "Failed to read data from file!\n", FILE_READ_ERR_LEN);
            exit(6);
        }
        for (int j = 0; j < tail_size; ++j) {
            if (cur_number != buffer[j]) {
                close(fd_read);
                write(2, "File data error!\n", FILE_DATA_ERR_LEN);
                exit(7);
            }
            cur_number = generate_next(cur_number);
        }
    }
    printf("All right\n");
    close(fd_read);
    
    exit(0);
}