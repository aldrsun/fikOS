#include <frame_buffer.h>
#include <io.h>

void fb_putchar_offset(struct framebuffer* fb, int offset, char ch, enum COLORS f_color, enum COLORS b_color) {
  //  char *ch_place = (char *) 0x000B8000 + 2 * offset;
  char *ch_place = (char *) fb->addr + 2 * offset;
  *ch_place = ch;
  *(ch_place + 1) = ((b_color & 0x0F) << 4) | (f_color & 0x0F);
}

void fb_putchar_cr(struct framebuffer* fb, int column, int row, char ch, enum COLORS f_color, enum COLORS b_color) {
  unsigned int offset = row * 80 + column;
  fb_putchar_offset(fb, offset, ch, f_color, b_color);
}

void fb_updatecursor(struct framebuffer* fb) {
  while (fb->cursor_offset > 25*80)
    fb->cursor_offset -= 25*80;
  while (fb->cursor_offset < 0)
    fb->cursor_offset ++;

  outb(0x3D4, 14);
  outb(0x3D5, (fb->cursor_offset >> 8) & 0x00F);
  outb(0x3D4, 15);
  outb(0x3D5, fb->cursor_offset & 0x00FF);
}

void fb_movecursor_nextline(struct framebuffer* fb) {
  int current_line_remaining = 80 - fb->cursor_offset % 80;
  for (int i = 0; i < current_line_remaining; i ++)
    fb_write(fb, " ");
}

void fb_movecursor_offset(struct framebuffer* fb, int offset) {
  fb->cursor_offset += offset;
  fb_updatecursor(fb);
}
void fb_movecursor_cr(struct framebuffer* fb, int column, int row) {
  fb->cursor_offset += column + 80 * row;
  fb_updatecursor(fb);
}

void fb_print_hex(struct framebuffer* fb, unsigned int value) {
  char buf[9];
  const char* hex = "0123456789ABCDEF";
  for (int i = 7; i >= 0; i--) {
    buf[i] = hex[value & 0xF];
    value >>= 4;
  }
  buf[8] = '\0';
  fb_write(fb, "0x");
  fb_write(fb, buf);

}
void fb_write(struct framebuffer* fb, const char *ch) {
  int index = 0;
  while (ch[index] != '\0') {
    fb_putchar_offset(fb, (fb->cursor_offset + index) % (25*80), ch[index], white, black);
    index ++;
  }
  fb_movecursor_offset(fb, index);
}

void fb_write_size(struct framebuffer* fb, const char *ch, int size) {

  for (int i = 0; i < size; i ++) {
    fb_putchar_offset(fb, (fb->cursor_offset + i) % (25*80), ch[i], white, black);
  }

  fb_movecursor_offset(fb, size);
}
