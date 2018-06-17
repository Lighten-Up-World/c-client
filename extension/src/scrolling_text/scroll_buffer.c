#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../utils/csv.h"
#include "../utils/list.h"
#include "../../../src/utils/error.h"
#include "../opc/opc_client.c"
#include "scroll_buffer.h"
#include "../extension.h"

volatile int interrupted = 0;

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
      b->grid[0][y] = (opc_pixel_t) {255, 255, 255};
    }
  }
}

void buffer_free(buffer_t* b) {
  grid_free(b->grid);
  free(b);
}

void shift_columns(opc_pixel_t **pixel_grid, buffer_t* buff) {
  // Store leftmost column temporarily
  opc_pixel_t left_col[GRID_HEIGHT];
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

// Updates opc_pixel_t list based on grid - more efficient to do this backwards iterating over the opc_pixel_t list
void read_grid_to_list(opc_pixel_t *pixel_list, opc_pixel_t **pixel_grid, list_t *pixel_info) {
  int pos;
  for (uint8_t x = 0; x < GRID_WIDTH; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      pos = get_pos(x, y, pixel_info);
      pixel_list[pos] = pixel_grid[x][y];
    }
  }
}


void handle_user_exit(int _) {
  printf("\n");
  interrupted = 1;
}

int init_grid(list_t* list){
  csv_parser_t *coords_to_pos_parser = csv_parser_new(PIXEL_FILE, " ");
  csv_row_t *row;
  while ((row = csv_parser_getRow(coords_to_pos_parser)) ) {
      char **rowFields = csv_parser_getFields(row);
      int pos = atoi(rowFields[2]);
      pixel_info_t *pi;
      if((pi = list_get(list, pos)) == NULL){
        pi = malloc(sizeof(pixel_info_t));
        *pi = (pixel_info_t){{.x = atoi(rowFields[0]), .y =atoi(rowFields[1])},
                {-1, -1}};
        list_add(list, pi); //TODO: NEEDS TO ADD IN CORRECT POS
      }
      else {
        pi->grid = (grid_t){.x = atoi(rowFields[0]), .y =atoi(rowFields[1])};
      }
      csv_parser_destroy_row(row);
  }
  csv_parser_destroy(coords_to_pos_parser);
  return 0;
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

  // Setup pixel_info
  list_t *pixel_info = list_new(&free);
  init_grid(pixel_info);

  opc_pixel_t pixels[NUM_PIXELS];
  opc_pixel_t **pixel_grid = pixel_grid_new();

  // Set opc_pixel_t grid to all white
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
    // Update the opc_pixel_t list
    read_grid_to_list(pixels, pixel_grid, pixel_info);

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
