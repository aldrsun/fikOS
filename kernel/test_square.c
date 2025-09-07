#include <keyboard.h>
#include <stdint.h>
#include <timer/timer.h>
#include <screen.h>

int square_x = 400, square_y = 400;
float square_speed = 200.0f;
float square_vx = 0.0f, square_vy = 0.0f;

// w -> 17
// a -> 30
// s -> 31
// d -> 32

void check_keypress() {
  square_vy = 0.0f;
  square_vx = 0.0f;
  if (get_is_pressed(17)) square_vy = -1.0f;
  if (get_is_pressed(31)) square_vy = 1.0f;
  if (get_is_pressed(30)) square_vx = -1.0f;
  if (get_is_pressed(32)) square_vx = 1.0f;
}

void position_update(uint32_t ms) {
  square_x += square_vx * square_speed * ((float)ms / 1000.0f);
  square_y += square_vy * square_speed * ((float)ms / 1000.0f);
  if (square_x < 0) square_x = 0;
  if (SCREEN_WIDTH < square_x + 100) square_x = SCREEN_WIDTH - 100;
  if (square_y < 0) square_y = 0;
  if (SCREEN_HEIGHT < square_y + 100) square_y = SCREEN_HEIGHT - 100;
}

char decimal_to_charptr_result[50];
void decimal_to_charptr(uint32_t decimal) {
  uint8_t size = 0;
  uint32_t tmp_decimal = decimal;
  while (tmp_decimal > 0) {
    tmp_decimal /= 10;
    size ++;
  }
  decimal_to_charptr_result[size] = 0;
  for(int i = size - 1; i >= 0; i --) {
    decimal_to_charptr_result[i] = '0' + decimal % 10;
    decimal /= 10;
  }
}

void test_square_run() {
  uint64_t last_ticks = get_ticks();
  while (1) {
    uint64_t current_ticks = get_ticks();
    uint64_t ticks_passed = current_ticks - last_ticks;
    last_ticks = current_ticks;

    check_keypress();

    position_update(ticks_passed);

    screen_fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x00D98B6C);

    screen_fill_rect(square_x, square_y, 100, 100, 0x00DE6916);

    screen_swap_buffers();
    //fsleep(100);
  }
}
