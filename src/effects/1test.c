#include "alltest.h"
#include "common.h"

// Light up the first pixel only
int test1_run(effect_runner_t* self) {
  for(int i = 0; i < NUM_PIXELS; i++){
    self->frame->pixels[i] = (opc_pixel_t) {50, 50, 50};
  }

  // Update the opc_pixel_t list
  self->frame->pixels[0] = WHITE_PIXEL;

  // Write the pixels to the display
  opc_put_pixel_list(self->sink, self->frame->pixels, self->pixel_info);

  return 0;
}

void free_test_effect2(effect_t *self){
  opc_pixel_t **grid = self->obj;
  if(self != NULL){
    grid_free(grid);
  }
  free_effect(self);
}

effect_t *get_test1_effect(void * obj){
  effect_t *effect = malloc(sizeof(effect_t));
  effect->time_delta = (struct timespec){0, 1000 * MILLI_TO_NANO};
  effect->run = &test1_run;
  effect->remove = &free_test_effect2;

  opc_pixel_t **pixel_grid = pixel_grid_new();

  // Set opc_pixel_t grid to all black
  for (uint8_t x = 0; x < GRID_WIDTH; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      pixel_grid[x][y] = BLACK_PIXEL;
    }
  }

  effect->obj = pixel_grid;

  return effect;
}
