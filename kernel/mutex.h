#ifndef MUTEX_H
#define MUTEX_H

struct mutex {
    int descriptors_num;
    int pid;
    struct sleeplock lock;
};

#endif