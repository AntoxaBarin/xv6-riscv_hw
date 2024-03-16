#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/procinfo.h"

#define PSLIST_ERR_LEN 35

int 
main(int argc, char* argv[]) {
    int array_size = 1;
    struct procinfo *pinfos = malloc(array_size * sizeof(struct procinfo));
    int ps_listinfo_code = ps_listinfo(pinfos, array_size);
    
    // increase array size while space is not enough 
    while (ps_listinfo_code == -1) {        
        free(pinfos);                  
        array_size *= 2;
        pinfos = malloc(array_size * sizeof(struct procinfo));  // realloc twice more memory
        ps_listinfo_code = ps_listinfo(pinfos, array_size);        
    }

    struct procinfo pinfo[array_size];
    int proc_num = ps_listinfo(pinfo, array_size);
    
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

    free(pinfos);
    exit(0);
}