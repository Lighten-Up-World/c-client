#include "scroll_buffer.h"

int main() {
  srand(42);
  // Create a buffer pixel grid to contain data about to be displayed
  int buff_width = 5;
  buffer_t *buff = buffer_new(buff_width);
  clear_buffer(buff);

  // Set a shaded buffer
  for (uint8_t x = 0; x < buff_width; x++){
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      int c = x * (255/buff_width);
      buff->grid[x][y] = (opc_pixel_t) {c, c, c};
    }
  }

  run(buff, 200);

  buffer_free(buff);

  return 0;
}
