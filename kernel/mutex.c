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
#include "mutex.h"

struct mutex mtable[NMUTEX];
struct spinlock mtable_lock;

void
mutex_init(void) {
    initlock(&mtable_lock, "mutex table spinlock");  // init mutex table global spinlock 
    
    acquire(&mtable_lock);                           // lock all table
    for (int i = 0 ; i < NMUTEX; i++) {
        mtable[i].descriptors_num = 0;
        initsleeplock(&mtable[i].lock, "mutex sleeplock"); // init mutex sleeplock
    }
    release(&mtable_lock);
}

int
mutex_lock(int mutex_desc) {
    if (holdingsleep(&mtable[mutex_desc].lock)) {
        return -1;      // mutex already locked
    }
    
    acquire(&mtable_lock);
    if (mtable[mutex_desc].descriptors_num == 0) {
        release(&mtable_lock);
        return -2;      // mutex isn't used by any process
    }
    release(&mtable_lock);
    acquiresleep(&mtable[mutex_desc].lock);
    
    return 0;           // successfully locked mutex
}

int
mutex_unlock(int mutex_desc) {
    if (holdingsleep(&mtable[mutex_desc].lock) == 0) {
        return -1;      // mutex already unlocked
    }
    
    acquire(&mtable_lock);
    if (mtable[mutex_desc].descriptors_num == 0) {
        release(&mtable_lock);
        return -2;      // mutex isn't used by any process
    }
    release(&mtable_lock);
    releasesleep(&mtable[mutex_desc].lock);

    return 0;           // successfully unlocked mutex
}

int 
mutex_create(void) {
    struct proc *p = myproc();
    int created_mutex_desc = -1;

    acquire(&mtable_lock); // lock mutex table
    for (int i = 0; i < NMUTEX; i++) {
        if (mtable[i].descriptors_num == 0) {
            mtable[i].descriptors_num += 1;
            created_mutex_desc = i;
            break;
        }
    }
    release(&mtable_lock);

    if (created_mutex_desc != -1) {
        for (int i = 0; i < NOMUTEX; i++) {
            if (p->omutex[i] == 0) {
                p->omutex[i] = &mtable[created_mutex_desc];
                return created_mutex_desc;  
            } 
        }
        return -1;     // too many mutexes per one proc
    }
    else {
        return -2;     // all mutexes are busy
    }
}

int
mutex_destroy(int mutex_desc) {
    acquire(&mtable_lock);
    if (mtable[mutex_desc].descriptors_num == 0) {
        release(&mtable_lock);
        return -1; // nothing to destroy
    }

    struct proc *p = myproc();
    for (int i = 0; i < NOMUTEX; i++) {
        if (p->omutex[i] == &mtable[mutex_desc]) {
            mtable[mutex_desc].descriptors_num -= 1;
            
            release(&mtable_lock);
            p->omutex[i] = 0;
            return 0;     // successfully destroy mutex for proc p           
        }    
    } 
    release(&mtable_lock);
    return -2;    // no mutex in mutexes of proc p
}


