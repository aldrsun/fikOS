#include <multiboot2.h>
#include <acpi/acpi.h>

struct multiboot_tag_framebuffer multiboot_framebuffer_tag;

struct multiboot_tag_framebuffer *get_framebuffer_tag() {
  return &multiboot_framebuffer_tag;
}

void init_multiboot2(unsigned long multiboot_info_addr) {
  // Start parsing after the initial 8-byte header
  struct multiboot_tag* tag = (struct multiboot_tag*)(multiboot_info_addr + 8);

  while (tag->type != 0) {
    switch (tag->type) {
      // Framebuffer Tag
    case 8:
      {
	multiboot_framebuffer_tag  = *(struct multiboot_tag_framebuffer*)tag;
      }
      break;
      // ACPI 1.0 Tag
    case 14:
      {
	struct multiboot_tag_old_acpi* old_acpi_tag = (struct multiboot_tag_old_acpi*)tag;
	acpi_1_0_init(old_acpi_tag);
      }
      break;
      // ACPI 2.0+ Tag
    case 15:
      {
      	struct multiboot_tag_new_acpi* new_acpi_tag = (struct multiboot_tag_new_acpi*)tag;
	acpi_2_0plus_init(new_acpi_tag);
      }
      break;
    default:
      break;
    }
    // Move to the next tag, ensuring 8-byte alignment
    tag = (struct multiboot_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7));
  }
}
