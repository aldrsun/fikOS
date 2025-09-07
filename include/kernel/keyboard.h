#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_init();

int get_is_pressed(uint8_t keycode);

#endif
