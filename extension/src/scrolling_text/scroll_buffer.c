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
 * Get the pixel_t number from an x, y coordinate, according to the configuration file
 * Configuration file should be of format: x y #\n
 *
 * @param x: x location of pixel_t on map
 * @param y: y location of pixel_t on map
 * @param config_file: the file with configuration data
 * @return: the pixel_t number
 */
int get_pixel_location(uint8_t x, uint8_t y, const char *config_file) {
  FILE *file = fopen(config_file, "r");
  if (file == NULL) {
    perror("File could not be opened");
    exit(EC_SYS);
  }

  int location = -1;
  size_t line_size = 100 * sizeof(char); //shouldn't be larger than this, should calculate and move to global constant
  char *buffer =  (char *) malloc(line_size);
  while (fgets(buffer, (int) line_size, file) != NULL) {
    // Get x, y coordinate
    // If this is the coordinate searched for, return the pixel_t number
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

pixel_t **grid_new(int cols, int rows) {
  pixel_t **matrix;
  matrix = (pixel_t **) malloc(cols * sizeof(pixel_t *));
  if (!matrix) {
    return NULL;
  }

  matrix[0] = (pixel_t *) malloc(rows * cols * sizeof(pixel_t));
  if (!matrix[0]) {
    free(matrix);
    return NULL;
  }

  for (uint8_t i = 1; i < cols; i++) {
    matrix[i] = matrix[0] + i*rows;
  }
  return matrix;
}

pixel_t **pixel_grid_new() {
  return grid_new(GRID_WIDTH, GRID_HEIGHT);
}

buffer_t* buffer_new(int cols) {
  buffer_t* b = (buffer_t* ) malloc(sizeof(buffer_t));
  if (!b) {
    return NULL;
  }
  b->pos = 0;
  b->grid = grid_new(cols, GRID_HEIGHT);
  b->width = cols;
  return b;
}

void clear_buffer(buffer_t* b) {
  for (uint8_t x = 0; x < b->width; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      b->grid[0][y] = (pixel_t) {255, 255, 255};
    }
  }
}

void grid_free(pixel_t **pixel_grid) {
  free(pixel_grid[0]);
  free(pixel_grid);
}

void buffer_free(buffer_t* b) {
  grid_free(b->grid);
  free(b);
}

void shift_columns(pixel_t **pixel_grid, buffer_t* buff) {
  // Store leftmost column temporarily
  pixel_t left_col[GRID_HEIGHT];
  for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
    left_col[y] = pixel_grid[0][y];
  }

  // Shift all pixels left one
  for (uint8_t x = 0; x < GRID_WIDTH - 1; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      pixel_grid[x][y] = pixel_grid[x + 1][y];
    }
  }

  // Update rightmost column from buffer_tt hen update the buffer
  for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
    if(buff->pos < buff->width){
      pixel_grid[GRID_WIDTH - 1][y] = buff->grid[buff->pos][y];
    }
    else{
      pixel_grid[GRID_WIDTH - 1][y] = left_col[y];
    }
  }

  buff->pos++;
  if(buff->pos == buff->width && buff->width > GRID_WIDTH){
    buff->pos = 0;
  }
  if(buff->pos == GRID_WIDTH && buff->width < GRID_WIDTH){
    buff->pos = 0;
  }
}

// Updates pixel_t list based on grid - more efficient to do this backwards iterating over the pixel_t list
void read_grid_to_list(pixel_t *pixel_list, pixel_t **pixel_grid) {
  int pos;
  for (uint8_t x = 0; x < GRID_WIDTH; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      pos = get_pixel_location(x, y, CONFIG_FILE);
      pixel_list[pos] = pixel_grid[x][y];
    }
  }
}

void handle_user_exit(int _) {
  printf("\n");
  interrupted = 1;
}

// TODO: make sure we don't have a map sized gap written to the buffer_tb efore repeating
// TODO: make a parameter for delay before replaying the buffer
/**
 * Scroll a buffer_ta cross the map continuously
 *
 * @param buff: a buffer_tc ontaining the data to scroll across the map
 * @param rate: the delay between each frame of scrolling, in microseconds
 */
void run(buffer_t* buff, double rate) {
  // Copy buffer_ti nto new pointer
  // Not sure why we have to do this but we do
  // buffer_t* buff_copy = buffer_new(buff->width);
  // for (uint8_t x = 0; x < buff->width; x++) {
  //   for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
  //     buff_copy->grid[x][y] = buff->grid[x][y];
  //   }
  // }

  pixel_t pixels[NUM_PIXELS];
  pixel_t **pixel_grid = pixel_grid_new();

  // Set pixel_t grid to all white
  for (uint8_t x = 0; x < GRID_WIDTH; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
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
    // Update the pixel_t list
    read_grid_to_list(pixels, pixel_grid);

    // Write the pixels to the display
    opc_put_pixels(s, 0, NUM_PIXELS, pixels);
    nanosleep(&t1, &t2);

    // Scroll along 1
    shift_columns(pixel_grid, buff);
  }

  // Cleanup code
  grid_free(pixel_grid);
  buffer_free(buff);
  opc_close(s);
}
