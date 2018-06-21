#include "test.h"
#include "common.h"

/**
 * Scroll a buffer_ta cross the map continuously
 *
 * @param buff: a buffer_tc ontaining the data to scroll across the map
 * @param rate: the delay between each frame of scrolling, in microseconds
 */
int test0_run(effect_runner_t* self) {  //
  // opc_pixel_t **grid = self->effect->obj;
  // Assign interrupt handler to close connection and cleanup after early exit
  // opc_put_pixels(self->sink, 0, NUM_PIXELS, self->frame->pixels);
  //nanosleep(&self->effect->time_delta, NULL);

  for(int i = 0; i < NUM_PIXELS; i++){
    self->frame->pixels[i] = (opc_pixel_t){50, 50, 50};
  }
  //
  // int x = self->frame_no % GRID_WIDTH;
  // int y = (self->frame_no / GRID_WIDTH) % GRID_HEIGHT;
  // printf("Turning on (%d,%d)\n", x, y);
  // grid[x][y] = WHITE_PIXEL;
  //
  // // Update the opc_pixel_t list
  // read_grid_to_list(self->frame->pixels, grid, self->pixel_info);
  self->frame->pixels[0] = WHITE_PIXEL;
  // Write the pixels to the display
  opc_put_pixel_list(self->sink, self->frame->pixels, self->pixel_info);

  return 0;
}

void free_test_effect(effect_t *self){
  opc_pixel_t **grid = self->obj;
  if(self != NULL){
    grid_free(grid);
  }
  free_effect(self);
}

effect_t *get_test0_effect(void * obj){
  effect_t *effect = malloc(sizeof(effect_t));
  effect->time_delta = (struct timespec){0, 1000 * MILLI_TO_NANO};
  effect->run = &test_run;
  effect->remove = &free_test_effect;

  opc_pixel_t **pixel_grid = pixel_grid_new();

  // Set opc_pixel_t grid to all white
  for (uint8_t x = 0; x < GRID_WIDTH; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      pixel_grid[x][y] = BLACK_PIXEL;
    }
  }

  effect->obj = pixel_grid;

  return effect;
}
