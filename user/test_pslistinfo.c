#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/procinfo.h"

#define NULL 0

int 
null_address_test(void) {
    int ps_listinfo_code = ps_listinfo(NULL, NPROC);
    return ps_listinfo_code > 0;
}

int 
small_buffer_test(void) {
    struct procinfo pinfos[1];
    int ps_listinfo_code = ps_listinfo(pinfos, 1);
    return ps_listinfo_code == -1;
}

int
correct_work_test() {
    struct procinfo pinfos[NPROC];
    int ps_listinfo_code = ps_listinfo(pinfos, NPROC);
    return ps_listinfo_code > 0;
} 

int
main(int argc, char* argv[]) {
    int passed_test = 0;
    passed_test += null_address_test();
    passed_test += small_buffer_test();
    passed_test += correct_work_test();

    if (passed_test == 3) {
        printf("All tests passed!\n");
    }
    else {
        printf("Some tests are not passed!\n");
    }

    exit(0);
}