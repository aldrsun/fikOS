#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// get_framebuffer_tag
#include <multiboot2.h>

void screen_init();

void screen_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);

void screen_putchar_cr(char ch, uint32_t x, uint32_t y, uint32_t size, uint32_t color);
void screen_print(char *str, uint32_t x, uint32_t y, uint32_t size, uint32_t color);

void screen_swap_buffers();

#endif
