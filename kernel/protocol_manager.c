#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"
#include "file.h"
#include "protocol_manager.h"

struct prot_mng prot_mng;

void
prot_mng_init() {
    // Turn on all events
    prot_mng.modes[0] = 1;
    prot_mng.modes[1] = 0;
    prot_mng.modes[2] = 0;
    prot_mng.modes[3] = 1;

    // No tick limit
    prot_mng.last_tick = 0;
}

int   
prot_check(int event_type) { // Check is it allowed to protocol
    if (prot_mng.last_tick == 0) {
        return (prot_mng.modes[event_type] == 1) ? 0 : -1;
    }
    int cur_tick = ticks;
    
    if (cur_tick <= prot_mng.last_tick) {
        return 0;
    }
    return -1;
    
}

uint64  
sys_protmng(void) {    
    int event_type, turn_on, ticks_;
    argint(0, &event_type);
    argint(1, &turn_on);
    argint(2, &ticks_);

    if (ticks_ == 0) {
        prot_mng.last_tick = 0;  // no limit in ticks
    }
    else if (ticks_ > 0) {
        prot_mng.last_tick = ticks + ticks_; // ticks_ -- number of ticks we want to protocoling
    }

    prot_mng.modes[event_type - 1] = turn_on;

    return 0;
}