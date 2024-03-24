#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_mutex_create(void) {
    return mutex_create();
}

uint64
sys_mutex_destroy(void) {
    int mutex_desc;
    argint(0, &mutex_desc);
    return mutex_destroy(mutex_desc);
}

uint64
sys_mutex_lock(void) {
    int mutex_desc;
    argint(0, &mutex_desc);
    return mutex_lock(mutex_desc);
}

uint64
sys_mutex_unlock(void) {
    int mutex_desc;
    argint(0, &mutex_desc);
    return mutex_unlock(mutex_desc);
}