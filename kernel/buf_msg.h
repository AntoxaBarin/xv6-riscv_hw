#ifndef BUF_MSG_H
#define BUF_MSG_H

struct buf_msg {
    char buffer[BUFSIZE];
    int head;
    int tail;
    struct spinlock lock;
};

#endif