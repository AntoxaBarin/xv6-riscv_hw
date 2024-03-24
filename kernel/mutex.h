#ifndef MUTEX_H
#define MUTEX_H

struct mutex {
    int descriptors_num;
    struct sleeplock lock;
};

#endif