#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64 
sys_add(void) {
    int fst_number, snd_number;
    argint(0, &fst_number);
    argint(1, &snd_number);
    
    return (fst_number + snd_number);
}