#ifndef PROCINFO_H
#define PROCINFO_H

enum proc_state {
    STATE_UNUSED,
    STATE_USED,
    STATE_SLEEPING,
    STATE_RUNNABLE,
    STATE_RUNNING,
    STATE_ZOMBIE
};

struct procinfo {
    char name[16];
    int parent_pid;
    enum proc_state state;        
};

#endif