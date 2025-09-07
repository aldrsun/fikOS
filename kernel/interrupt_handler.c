#include <interrupt_handler.h>
#include <frame_buffer.h>
#include <io.h>
#include <stdint.h>
#include <timer/timer.h>

extern void (*interrupt_handler_pointers[256])();

// Struct for an individual IDT entry
struct idt_entry {
  unsigned short int offset_low;
  unsigned short int selector;
  unsigned char  always0;
  unsigned char  flags;
  unsigned short int offset_high;
} __attribute__((packed));

// Struct for the IDT pointer
struct idt_ptr {
  unsigned short int limit;
  unsigned int base;
} __attribute__((packed));

// The actual IDT array and its pointer
struct idt_entry idt[256];
struct idt_ptr idt_ptr_variable;

// Assembly function to load the IDT
extern void lidt_function_name(void);

// Function to set up an IDT entry
void idt_set_gate(unsigned char num, unsigned int base, unsigned short int sel, unsigned char flags) {
  idt[num].offset_low  = base & 0xFFFF;
  idt[num].offset_high = (base >> 16) & 0xFFFF;
  idt[num].selector    = sel;
  idt[num].always0     = 0;
  idt[num].flags       = flags;
}

// Main function to initialize and load the IDT
void idt_init() {
  // Populate IDT pointer
  idt_ptr_variable.limit = (sizeof(struct idt_entry) * 256) - 1;
  idt_ptr_variable.base = (unsigned int)&idt;

  // Set up a few gates, for example, for the interrupt handlers you defined in assembly
  // The selector (0x08) is the common kernel code segment selector in a GDT
  // The flags (0x8E) indicate a 32-bit interrupt gate (P=1, DPL=0, S=0, Type=1110b)

  for (int i = 0; i < 256; i++) {
    // Use the function pointer array to get the address of the correct handler.
    unsigned int handler_address = (unsigned int)interrupt_handler_pointers[i];
    idt_set_gate(i, handler_address, 0x08, 0x8E);
  }

  // Load the IDT using the assembly function
  lidt_function_name();
}

void pic_init() {
  // Remap the PIC
  outb(0x20, 0x11);
  outb(0xA0, 0x11);
  outb(0x21, 0x20); // Remap master PIC to interrupt 32
  outb(0xA1, 0x28); // Remap slave PIC to interrupt 40
  outb(0x21, 0x04);
  outb(0xA1, 0x02);
  outb(0x21, 0x01);
  outb(0xA1, 0x01);

  // Unmask keyboard IRQ (IRQ 1)
  outb(0x21, inb(0x21) & 0b11111101);
  outb(0xA1, inb(0xA1));
}

void interrupt_init() {
  idt_init();
  pic_init();
}

struct cpu_state {
  unsigned int edi;
  unsigned int esi;
  unsigned int ebp;
  unsigned int esp_dummy; // Not a real register, but `pushad` pushes ESP
  unsigned int ebx;
  unsigned int edx;
  unsigned int ecx;
  unsigned int eax;
} __attribute__((packed));

struct stack_state {
  unsigned int error_code;
  unsigned int eip;
  unsigned int cs;
  unsigned int eflags;
} __attribute__((packed));


extern void draw_that_rect();

// timer interrupt and keyboard interrupt (i.e., IN32 & IN33) is being handled in timer.c and keyboard.c directly, it never gets here
void interrupt_handler(struct cpu_state registers, unsigned int interrupt_number, struct stack_state stackstate) {
  if(registers.eax && stackstate.error_code) {
  }

  if (interrupt_number >= 32 && interrupt_number <= 47) {
    if (interrupt_number >= 40) {
      // IRQ came from slave PIC
      outb(0xA0, 0x20);   // EOI to slave
    }
    outb(0x20, 0x20);       // Always EOI to master
  }

  /*
    if (interrupt_number != 32) {
    do {
    const char *ali = "Ali. ";
    fb_write_size(ali, 5);
    } while(0);
    }
  */

}
