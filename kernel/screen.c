#include <stdint.h>

#include <screen.h>
#include <multiboot2.h>
#include <font.h>

struct multiboot_tag_framebuffer screen_framebuffer_tag;

uint32_t screen_canvas_buffer[SCREEN_WIDTH * SCREEN_HEIGHT * 4];

uint32_t screen_pitch_pixels;

void screen_init() {
  screen_framebuffer_tag = *get_framebuffer_tag();

  screen_pitch_pixels = screen_framebuffer_tag.pitch / 4;
}

void screen_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
  for (uint32_t row = 0; row < height; row++) {
    uint32_t offset = (y + row) * screen_pitch_pixels + x;
    for (uint32_t col = 0; col < width; col++) {
      screen_canvas_buffer[offset + col] = color;
    }
  }
}

void screen_putchar_cr(char ch, uint32_t x, uint32_t y, uint32_t size, uint32_t color) {
  char *font = get_font();

  for (uint32_t row = 0; row < 16 * size; row++) {
    uint32_t offset = (y + row) * screen_pitch_pixels + x;
    for (uint32_t col = 0; col < 8 * size; col++) {
      if (font[(ch * 16 * 8) + (row / size) * 8 + (col / size)] == '1')
	screen_canvas_buffer[offset + col] = color;
    }
  }

}

void screen_print(char *str, uint32_t x, uint32_t y, uint32_t size, uint32_t color) {
  uint32_t index = 0;
  while (*str != '\0') {
    screen_putchar_cr(*str, x + index * size, y, size, color);
    index += 8;
    str ++;
  }
}

void screen_swap_buffers() {
  uint32_t *screen_framebuffer_address = (void *)(uint32_t)screen_framebuffer_tag.addr;
  for (uint32_t row = 0; row < SCREEN_HEIGHT; row ++) {
    uint32_t offset = row * screen_pitch_pixels;
    for (uint32_t col = 0; col < SCREEN_WIDTH; col ++) {
      screen_framebuffer_address[offset + col] = screen_canvas_buffer[offset + col];
    }
  }
}
