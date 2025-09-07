#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void pit_init(uint32_t frequency);

void fsleep(uint64_t ticks);

uint64_t get_ticks();
#endif
