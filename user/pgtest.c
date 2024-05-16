#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUF_SIZE 1024

int main() {
    int* buf = malloc(BUF_SIZE * sizeof(int));
    int status;

    // Test 1: No access before any changes
    pgaccess(buf, sizeof(int) * BUF_SIZE, &status);

    if (status == 0) {
        printf("Test 1: success.\n");
    }
    else {
        printf("Test 1: fail.\n");
    }

    // Test 2: Access first element
    buf[0] = 0;
    pgaccess(buf, sizeof(int), &status);

    if (status == 1) {
        printf("Test 2: success.\n");
    }
    else {
        printf("Test 2: fail.\n");
    }

    // Test 3: Double pgaccess check
    buf[128] = 0;
    pgaccess(buf + 128, sizeof(int), &status);  // first access check --> status = 1 
    pgaccess(buf + 128, sizeof(int), &status);  // second access check --> status = 0

    if (status == 0) {
        printf("Test 3: success.\n");
    }
    else {
        printf("Test 3: fail.\n");
    }


    // Test 4: Stack variable check
    int var;
    pgaccess(&var, sizeof(int), 0);     // turn off page's access atribute 

    var = 42;
    pgaccess(&var, sizeof(int), &status);

    if (status == 1) {
        printf("Test 4: success.\n");
    }
    else {
        printf("Test 4: fail.\n");
    }
  
    // Test 5: Double pgaccess check (stack)
    pgaccess(&var, sizeof(int), 0);     // turn off page's access atribute 

    var = 1024;
    pgaccess(&var, sizeof(int), &status);
    pgaccess(&var, sizeof(int), &status);

    if (status == 0) {
        printf("Test 5: success.\n");
    }
    else {
        printf("Test 5: fail.\n");
    }


    printf("All tests passed\n");
    free(buf);
    exit(0);
}