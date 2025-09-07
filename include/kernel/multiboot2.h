#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include <stdint.h>

// General Multiboot tag header
struct multiboot_tag {
  uint32_t type;
  uint32_t size;
} __attribute__((packed));

// Framebuffer tag data structure
struct multiboot_tag_framebuffer {
  uint32_t type;
  uint32_t size;
  uint64_t addr;
  uint32_t pitch;
  uint32_t width;
  uint32_t height;
  uint8_t bpp;
  uint8_t framebuffer_type;
  uint8_t reserved;
} __attribute__((packed));

// For ACPI 1.0 RSDP (Type 14)
struct multiboot_tag_old_acpi {
  uint32_t type; // = MBT_ACPI_OLD (14)
  uint32_t size; // = 16
  uint8_t rsdp[0]; // The RSDP structure starts here. For v1.0, it's 20 bytes.
} __attribute__((packed)); // The actual RSDP data is stored *in-place* in the tag.

// For ACPI 2.0+ RSDP (Type 15)
struct multiboot_tag_new_acpi {
  uint32_t type; // = MBT_ACPI_NEW (15)
  uint32_t size; // = 24
  uint8_t rsdp[0]; // The RSDP structure starts here. For v2.0+, it's 36 bytes.
} __attribute__((packed));

void init_multiboot2(unsigned long multiboot_info_addr);

// GLOBAL VARIABLES
struct multiboot_tag_framebuffer *get_framebuffer_tag();

#endif
