#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../../../src/utils/error.h"
#include "../opc/opc_client.c"
#include "scroll_buffer.h"

volatile int interrupted = 0;

/**
 * Get the pixel number from an x, y coordinate, according to the configuration file
 * Configuration file should be of format: x y #\n
 *
 * @param x: x location of pixel on map
 * @param y: y location of pixel on map
 * @param config_file: the file with configuration data
 * @return: the pixel number
 */
int get_pixel_location(uint8_t x, uint8_t y, const char *config_file) {
  FILE *file = fopen(config_file, "r");
  if (file == NULL) {
    perror("File could not be opened");
    exit(EC_SYS);
  }

  int location = -1;
  size_t line_size = 100 * sizeof(char); //shouldn't be larger than this, should calculate and move to global constant
  char *buffer = (char *) malloc(line_size);
  while (fgets(buffer, (int) line_size, file) != NULL) {
    // Get x, y coordinate
    // If this is the coordinate searched for, return the pixel number
    if (atoi(strtok(buffer, " ")) == x) {
      if (atoi(strtok(NULL, " ")) == y) {
        location = atoi(strtok(NULL, " "));
        goto cleanup;
      }
    }
  }

  if (ferror(file)) {
    free(buffer);
    perror("Failed to read from file");
    exit(EC_SYS);
  }

  cleanup:
  if (fclose(file)) {
    free(buffer);
    perror("File could not be closed");
    exit(EC_SYS);
  }

  free(buffer);
  return location;
}

pixel **grid_new(int cols, int rows) {
  pixel **matrix;
  matrix = (pixel **) malloc(cols * sizeof(pixel *));
  if (!matrix) {
    return NULL;
  }

  matrix[0] = (pixel *) malloc(rows * cols * sizeof(pixel));
  if (!matrix[0]) {
    free(matrix);
    return NULL;
  }

  for (uint8_t i = 1; i < cols; i++) {
    matrix[i] = matrix[i - 1] + rows;
  }
  return matrix;
}

pixel **pixel_grid_new() {
  return grid_new(COLS, ROWS);
}

buffer *buffer_new(int cols) {
  buffer *b = (buffer *) malloc(sizeof(buffer));
  if (!b) {
    return NULL;
  }
  b->grid = grid_new(cols, ROWS);
  b->width = cols;
  return b;
}

void clear_buffer(buffer *b) {
  for (uint8_t x = 0; x < b->width; x++) {
    for (uint8_t y = 0; y < ROWS; y++) {
      b->grid[0][y] = (pixel) {255, 255, 255};
    }
  }
}

void grid_free(pixel **pixel_grid) {
  free(pixel_grid[0]);
  free(pixel_grid);
}

void buffer_free(buffer *b) {
  grid_free(b->grid);
  free(b);
}

void shift_columns(pixel **pixel_grid, buffer *buff) {
  // Store leftmost column temporarily
  pixel left_col[ROWS];
  for (uint8_t y = 0; y < ROWS; y++) {
    left_col[y] = pixel_grid[0][y];
  }

  // Shift all pixels left one
  for (uint8_t x = 0; x < COLS - 1; x++) {
    for (uint8_t y = 0; y < ROWS; y++) {
      pixel_grid[x][y] = pixel_grid[x + 1][y];
    }
  }

  // Update rightmost column from buffer then update the buffer
  for (uint8_t y = 0; y < ROWS; y++) {
    pixel_grid[COLS - 1][y] = buff->grid[0][y];
    buff->grid[buff->width - 1][y] = left_col[y];
  }
}

// Updates pixel list based on grid - more efficient to do this backwards iterating over the pixel list
void read_grid_to_list(pixel *pixel_list, pixel **pixel_grid) {
  int pos;
  for (uint8_t x = 0; x < COLS; x++) {
    for (uint8_t y = 0; y < ROWS; y++) {
      pos = get_pixel_location(x, y, CONFIG_FILE);
      pixel_list[pos] = pixel_grid[x][y];
    }
  }
}

void handle_user_exit(int _) {
  interrupted = 1;
}

// TODO: make sure we don't have a map sized gap written to the buffer before repeating
// TODO: make a parameter for delay before replaying the buffer
/**
 * Scroll a buffer across the map continuously
 *
 * @param buff: a buffer containing the data to scroll across the map
 * @param rate: the delay between each frame of scrolling, in milliseconds
 */
void run(buffer *buff, double rate) {

  pixel pixels[PIXELS];
  pixel **pixel_grid = pixel_grid_new();

  // Set pixel grid to all white
  for (uint8_t x = 0; x < COLS; x++) {
    for (uint8_t y = 0; y < ROWS; y++) {
      pixel_grid[x][y] = WHITE_PIXEL;
    }
  }

  // Open connection
  opc_sink s = opc_new_sink(HOST_AND_PORT);

  // Setup the time delay
  struct timespec t1, t2;
  t1.tv_sec = 0;
  t1.tv_nsec = (long) (rate * MILLI_TO_NANO);

  // Assign interrupt handler to close connection and cleanup after early exit
  signal(SIGINT, handle_user_exit);

  while (!interrupted) {
    // Update the pixel list
    read_grid_to_list(pixels, pixel_grid);

    // Write the pixels to the display
    opc_put_pixels(s, 0, PIXELS, pixels);
    nanosleep(&t1, &t2);

    // Scroll along 1
    shift_columns(pixel_grid, buff);
  }

  // Cleanup code
  grid_free(pixel_grid);
  opc_close(s);
}
