#include <io.h>
#include <stdint.h>
#include <timer/timer.h>

static volatile uint64_t timer_ticks = 0;

#define PIT_FREQUENCY 1193182
#define PIT_COMMAND   0x43
#define PIT_CHANNEL0  0x40

void pit_init(uint32_t frequency) {
    uint32_t divisor = PIT_FREQUENCY / frequency;

    // Command byte: channel 0, low+high byte access, mode 2 (rate generator), binary mode
    outb(PIT_COMMAND, 0x36);

    // Send divisor low byte, then high byte
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

void timer_interrupt() {
  timer_ticks ++;
  outb(0x20, 0x20);       // Always EOI to master
}

void fsleep(uint64_t ticks) {
  uint64_t end = timer_ticks + ticks;
  while (timer_ticks < end) {
    __asm__ volatile("hlt"); // halt CPU until next interrupt
  }
}

uint64_t get_ticks() {
  return timer_ticks;
}
