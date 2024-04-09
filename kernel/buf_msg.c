#include <stdarg.h>
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
#include "buf_msg.h"

static char digits[] = "0123456789abcdef";

struct buf_msg buf_msg;

void 
buf_msg_init(void) {
    initlock(&buf_msg.lock, "message buffer spinlock");

    acquire(&buf_msg.lock);
    buf_msg.buffer[0] = '\n';  // prettier output
    buf_msg.head = 0;
    buf_msg.tail = 1;
    release(&buf_msg.lock);
}

void 
write_byte_into_buf_msg(char byte) {       // lock acquires in pr_msg(...)
    buf_msg.buffer[buf_msg.tail++] = byte;

    if (buf_msg.tail >= BUFSIZE) {
        buf_msg.tail %= BUFSIZE; 
    }
    if (buf_msg.tail <= buf_msg.head) {
        buf_msg.head = (buf_msg.head + 1) % BUFSIZE;
    }
}

void 
write_int_into_buf_msg(int num, int base, int sign) {  // implementation from printf.c
    char buf[16];
    int i;
    uint x;

    if (sign && (sign = num < 0))
        x = -num;
    else
        x = num;

    i = 0;
    do {
        buf[i++] = digits[x % base];
    } while ((x /= base) != 0);

    if (sign)
        buf[i++] = '-';

    while (--i >= 0)
        write_byte_into_buf_msg(buf[i]);
}

void  
write_ptr_into_buf_msg(uint64 ptr) {      // implementation from printf.c
    write_byte_into_buf_msg('0');
    write_byte_into_buf_msg('x');

    for (int i = 0; i < (sizeof(uint64) * 2); i++, ptr <<= 4) {
        write_byte_into_buf_msg(digits[ptr >> (sizeof(uint64) * 8 - 4)]);
    }
}

void 
write_fmt_into_buf_msg(const char *fmt, va_list ap) {   // implementation from printf.c
    printf("DEBUG: %s _ %s\n", fmt, "asdasd");
    int i, c;
    char *s;

    for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
        if (c != '%') {
            write_byte_into_buf_msg(c);
            continue;
        }
        c = fmt[++i] & 0xff;
        if (c == 0) {
            break;
        }
        switch (c) {
        case 'd':
            write_int_into_buf_msg(va_arg(ap, int), 10, 1);
            break;
        case 'x':
            write_int_into_buf_msg(va_arg(ap, int), 16, 1);
            break;
        case 'p':
            write_ptr_into_buf_msg(va_arg(ap, uint64));
            break;
        case 's':
            if ((s = va_arg(ap, char *)) == 0)
                s = "(null)";
            for (; *s; s++)
                write_byte_into_buf_msg(*s);
            break;
        case '%':
            write_byte_into_buf_msg('%');
            break;
        default:
            write_byte_into_buf_msg('%');
            write_byte_into_buf_msg(c);
            break;
        }
    }
    va_end(ap);
}

void pr_msg(const char *fmt, ...) {
    acquire(&buf_msg.lock);

    acquire(&tickslock);
    int ticks_num = ticks;  // get ticks
    release(&tickslock);

    va_list ap;
    va_start(ap, fmt);

    // write current tick as prefix of message
    write_byte_into_buf_msg('[');
    write_int_into_buf_msg(ticks_num, 10, 0);
    write_byte_into_buf_msg(']');
    write_byte_into_buf_msg(' ');

    // write format string
    write_fmt_into_buf_msg(fmt, ap);
    va_end(ap);

    write_byte_into_buf_msg('\n');
    release(&buf_msg.lock);
}

uint64
sys_dmesg(void) {
    acquire(&buf_msg.lock);
    char null[1] = "\0";

    uint64 dst;
    argaddr(0, &dst);

    if (buf_msg.tail > buf_msg.head) {  // [head ... tail]
        if (copyout(myproc()->pagetable, dst, buf_msg.buffer + buf_msg.head, sizeof(char) * (buf_msg.tail - buf_msg.head)) < 0) {
            release(&buf_msg.lock);
            return -1;
        }
        // add '\0'
        if (copyout(myproc()->pagetable, dst + sizeof(char) * (buf_msg.tail - buf_msg.head), null, sizeof(char)) < 0) {
            release(&buf_msg.lock);
            return -1;
        }
        release(&buf_msg.lock);
        return buf_msg.tail - buf_msg.head + 1;  // all messages size
    } 
    else {       // [..tail_head...]
        uint64 first_part_length = sizeof(char) * (BUFSIZE - buf_msg.head);
        if (copyout(myproc()->pagetable, dst, buf_msg.buffer + buf_msg.head, first_part_length) < 0) {   // [head....]
            release(&buf_msg.lock);
            return -1;
        }
        if (copyout(myproc()->pagetable, dst + first_part_length, buf_msg.buffer, sizeof(char) * buf_msg.tail) < 0) {  // [...tail]
            release(&buf_msg.lock);
            return -1;
        }
        // add '\0'
        if (copyout(myproc()->pagetable, dst + sizeof(char) * BUFSIZE, null, sizeof(char)) < 0) {
            release(&buf_msg.lock);
            return -1;
        }
    }

    release(&buf_msg.lock);
    return BUFSIZE + 1;       // all messages size
}
