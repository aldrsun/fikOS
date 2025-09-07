#include "acpi/acpi.h"
#include <keyboard.h>
#include <stdint.h>
#include <io.h>


#include <serial.h>

char kbd_US[128] =
  {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', /* <-- Tab */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, /* <-- control key */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
    '*',
    0,  /* Alt */
    ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
  };

uint8_t scancode;

uint8_t is_pressed[256];

void keyboard_init() {
  for (int i = 0; i < 256; i ++) {
    is_pressed[i] = 0;
  }
}

void keyboard_interrupt() {
  scancode = inb(0x60);
  uint8_t keycode = scancode & 0x7F; // delete the pressed released info, bit 7 (8th bit...)
  if (keycode == 1) { // esc
    acpi_shutdown();
  }
  if (scancode & 0x80) {
    // Key release
    is_pressed[keycode] = 0;
  } else {
    // Key press
    is_pressed[keycode] = 1;
  }

  /*
  serial_print_string("interrupt catched that ");
  if (keycode != scancode) {
    serial_print_string("release of ");
  } else {
    serial_print_string("press of ");
  }
  serial_print_byte(kbd_US[keycode]);
  serial_print_string("\n");
  */

  outb(0x20, 0x20);       // Always EOI to master
}

int get_is_pressed(uint8_t keycode) {
  return is_pressed[keycode];
}
