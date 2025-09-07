#include <kernel_init.h>

#include <test_square.h>

int kmain(unsigned int magic_number, unsigned long multiboot_info_addr) {
  if (magic_number != 0x36d76289) {
    // Not a valid Multiboot2 boot
    return -1;
  }

  kernel_init(multiboot_info_addr);

  test_square_run();

  return 1;
}
