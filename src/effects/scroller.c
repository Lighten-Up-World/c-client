#include "scroller.h"
#include "common.h"

buffer_t *buffer_new(int cols) {
  buffer_t *b = (buffer_t *) malloc(sizeof(buffer_t));
  if (!b) {
    return NULL;
  }
  b->pos = 0;
  b->grid = grid_new(cols, GRID_HEIGHT);
  b->width = cols;
  return b;
}

void clear_buffer(buffer_t *b, int width) {
  for (uint8_t x = 0; x < width; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      b->grid[0][y] = (opc_pixel_t) {255, 0, 255};
    }
  }
}

void buffer_free(buffer_t *b) {
  grid_free(b->grid);
  free(b);
}

void shift_columns(opc_pixel_t **pixel_grid, buffer_t *buff) {
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

  // Update rightmost column from buffer_t then update the buffer
  for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
    if (buff->pos < buff->width) {
      pixel_grid[GRID_WIDTH - 1][y] = buff->grid[buff->pos][y];
    } else {
      pixel_grid[GRID_WIDTH - 1][y] = left_col[y];
    }
  }

  //TODO: Buffer isn't consistent need to have an init and store in effect
  buff->pos++;
  if (buff->pos == buff->width && buff->width > GRID_WIDTH) {
    buff->pos = 0;
  }
  if (buff->pos == GRID_WIDTH && buff->width < GRID_WIDTH) {
    buff->pos = 0;
  }
}

// TODO: make sure we don't have a map sized gap written to the buffer_t before repeating
// TODO: make a parameter for delay before replaying the buffer
/**
 * Scroll a buffer_ta cross the map continuously
 *
 * @param buff: a buffer_tc ontaining the data to scroll across the map
 * @param rate: the delay between each frame of scrolling, in microseconds
 */
int scroller_run(effect_runner_t *self) {  //
  scroller_storage_t *storage = self->effect->obj;

  // Assign interrupt handler to close connection and cleanup after early exit
  // Update the opc_pixel_t list
  read_grid_to_list(self->frame->pixels, storage->pixel_grid, self->pixel_info);

  // Write the pixels to the display
  opc_put_pixel_list(self->sink, self->frame->pixels, self->pixel_info);

  // Scroll along 1
  shift_columns(storage->pixel_grid, storage->buff);
  return 0;
}

void free_scroller_storage(effect_t *self) {
  scroller_storage_t *storage = self->obj;
  if (storage != NULL) {
    grid_free(storage->pixel_grid);
    buffer_free(storage->buff);
  }
  free(storage);
}

void free_scroller(effect_t *self) {
  free_scroller_storage(self);
  free_effect(self);
}

effect_t *get_scroller_effect(void *obj) {
  effect_t *effect = calloc(1, sizeof(effect_t));
  if (effect == NULL) {
    return NULL;
  }
  effect->time_delta = (struct timespec) {0, 75 * MILLI_TO_NANO};
  effect->run = &scroller_run;
  effect->remove = &free_effect;

  // Create a buffer pixel grid to contain data about to be displayed
  int buff_width = 100;
  buffer_t *buff = buffer_new(buff_width);
  clear_buffer(buff, 100);

  // Set a shaded buffer
  for (uint8_t x = 60; x < buff_width; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      //int c = ((x - 60) / (buff_width - 60)) * 255;
      int c = x * (255 / (buff_width - 60));
      buff->grid[x][y] = (opc_pixel_t) {255, 0, c};
    }
  }

  // Set background buffer
  for (uint8_t x = 0; x < 60; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      buff->grid[x][y] = (opc_pixel_t) {255, 0, 255};
    }
  }

  // COMPLETE hack but I am way too tired
  for (uint8_t x = 85; x < 100; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      buff->grid[x][y] = (opc_pixel_t) {255, 0, 255};
    }
  }

  opc_pixel_t **pixel_grid = pixel_grid_new();

  // Set opc_pixel_t grid to all white
  for (uint8_t x = 0; x < GRID_WIDTH; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      pixel_grid[x][y] = (opc_pixel_t) {255, 0, 255};
    }
  }

  effect->obj = malloc(sizeof(scroller_storage_t));
  scroller_storage_t *storage = effect->obj;
  storage->buff = buff;
  storage->pixel_grid = pixel_grid;

  return effect;
}
