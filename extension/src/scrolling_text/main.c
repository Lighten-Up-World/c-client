#include "scroll_buffer.h"

int main() {
  // Create a buffer pixel grid to contain data about to be displayed
  buffer *buff = buffer_new(1);
  clear_buffer(buff);

  // Set a row of blue in buffer
  for (uint8_t y = 0; y < ROWS; y++) {
    buff->grid[0][y] = (pixel) {0, 0, 255};
  }

  run(buff, 1);

  buffer_free(buff);

  return 0;
}
