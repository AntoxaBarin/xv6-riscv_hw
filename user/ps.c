#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/procinfo.h"

#define PSLIST_ERR_LEN 35

int 
main(int argc, char* argv[]) {
    struct procinfo pinfo[NPROC];
    int proc_num = ps_listinfo(pinfo, NPROC);
    
    if (proc_num < 0) {
        write(2, "Error: failed to get procces info!\n", PSLIST_ERR_LEN);
        exit(1);
    }

    static char *proc_states[] = {
    [STATE_UNUSED]    "UNUSED ",
    [STATE_USED]      "USED ",
    [STATE_SLEEPING]  "SLEEPING ",
    [STATE_RUNNABLE]  "RUNNABLE ",
    [STATE_RUNNING]   "RUNNING ",
    [STATE_ZOMBIE]    "ZOMBIE "
  };
    
    for (int i = 0; i < proc_num; i++) {
        printf("%s\t%d\t%s\n", proc_states[pinfo[i].state], pinfo[i].parent_pid, pinfo[i].name);
    }

    exit(0);
}