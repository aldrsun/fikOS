#ifndef IO_H
#define IO_H

#include <stdint.h>

extern int outb(int addr, int data);
extern unsigned char inb(int addr);
extern void outw(uint16_t port, uint16_t value);
void test_write();

#endif
