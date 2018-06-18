#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../utils/csv.h"
#include "../utils/list.h"
#include "../../../src/utils/error.h"
#include "../opc/opc_client.h"
#include "scroller.h"
#include "../extension.h"

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
  //TODO: Buffer isn't consistent need to have an init and store in effect
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

// TODO: make sure we don't have a map sized gap written to the buffer_tb efore repeating
// TODO: make a parameter for delay before replaying the buffer
/**
 * Scroll a buffer_ta cross the map continuously
 *
 * @param buff: a buffer_tc ontaining the data to scroll across the map
 * @param rate: the delay between each frame of scrolling, in microseconds
 */
int run(effect_runner_t* self) {  //
  scroller_storage_t *storage = self->effect->obj;

  // // Setup the time delay
  // struct timespec t1, t2;
  // t1.tv_sec = 0;
  // t1.tv_nsec = (long) (rate * MILLI_TO_NANO);

  // Assign interrupt handler to close connection and cleanup after early exit
  // Update the opc_pixel_t list
  read_grid_to_list(self->frame->pixels, storage->pixel_grid, self->pixel_info);

  // Write the pixels to the display
  opc_put_pixels(self->sink, 0, NUM_PIXELS, self->frame->pixels);
  nanosleep(&self->effect->time_delta, NULL);

  // Scroll along 1
  shift_columns(storage->pixel_grid, storage->buff);

  // TODO: Add cleanup function to effect Cleanup code
  // grid_free(pixel_grid);
  // buffer_free(buff);
  return 0;
}

effect_t *get_scroller_effect(void){
  effect_t *effect = calloc(1, sizeof(effect_t));
  if(effect == NULL){
    return NULL;
  }
  effect->time_delta = (struct timespec){0, 50 * MILLI_TO_NANO};
  effect->run = &run;

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

  opc_pixel_t **pixel_grid = pixel_grid_new();

  // Set opc_pixel_t grid to all white
  for (uint8_t x = 0; x < GRID_WIDTH; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      pixel_grid[x][y] = WHITE_PIXEL;
    }
  }


  effect->obj = malloc(sizeof(scroller_storage_t));
  scroller_storage_t *storage = effect->obj;
  storage->buff = buff;
  storage->pixel_grid = pixel_grid;

  return effect;
}
