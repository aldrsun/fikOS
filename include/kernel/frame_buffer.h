enum COLORS {
  black = 0,
  blue = 1,
  green = 2,
  cyan = 3,
  red = 4,
  magenta = 5,
  brown = 6,
  light_grey = 7,
  dark_grey = 8,
  light_blue = 9,
  light_green = 10,
  light_cyan = 11,
  light_red = 12,
  light_magenta = 13,
  light_brown = 14,
  white = 15
};

struct framebuffer {
  void *addr;
  int cursor_offset;
};

void fb_putchar_offset(struct framebuffer* fb, int offset, char ch, enum COLORS f_color, enum COLORS b_color);
void fb_putchar_cr(struct framebuffer* fb, int column, int row, char ch, enum COLORS f_color, enum COLORS b_color);

void fb_movecursor_nextline(struct framebuffer* fb);
void fb_movecursor_offset(struct framebuffer* fb, int offset);
void fb_movecursor_cr(struct framebuffer* fb, int colum, int row);

void fb_print_hex(struct framebuffer* fb, unsigned int value);
void fb_write_size(struct framebuffer* fb, const char *ch, int size);
void fb_write(struct framebuffer* fb, const char *ch);
