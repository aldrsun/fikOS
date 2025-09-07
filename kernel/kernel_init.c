#include <multiboot2.h>
#include <kernel_init.h>
#include <interrupt_handler.h>
#include <timer/timer.h>
#include <keyboard.h>
#include <screen.h>

void kernel_init(unsigned long multiboot_info_addr) {
  init_multiboot2(multiboot_info_addr);

  interrupt_init();

  pit_init(1000);

  keyboard_init();

  screen_init();

  __asm__ volatile("sti");
}
