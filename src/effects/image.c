#include "common.h"
#include "image.h"

/**
 * Scroll a buffer_ta cross the map continuously
 *
 * @param buff: a buffer_tc ontaining the data to scroll across the map
 * @param rate: the delay between each frame of scrolling, in microseconds
 */
int image_run(effect_runner_t* self) {  //
  opc_pixel_t **pixel_grid = self->effect->obj;

  // Update the opc_pixel_t list
  read_grid_to_list(self->frame->pixels, pixel_grid, self->pixel_info);

  // Write the pixels to the display
  opc_put_pixels(self->sink, 0, NUM_PIXELS, self->frame->pixels);
  nanosleep(&self->effect->time_delta, NULL);

  return 0;
}

void free_image(effect_t *self){
  //TODO: FREE IMAGE GRID
  free_effect(self);
}

effect_t *get_image_effect(void *obj){
  effect_t *effect = calloc(1, sizeof(effect_t));
  if(effect == NULL){
    return NULL;
  }
  effect->time_delta = (struct timespec){0, 50 * MILLI_TO_NANO};
  effect->run = &image_run;
  effect->remove = &free_effect;

  effect->obj = pixel_grid_new();
  opc_pixel_t **pixel_grid = effect->obj;
  // Set opc_pixel_t grid to all white
  for (uint8_t x = 0; x < GRID_WIDTH; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      pixel_grid[x][y] = WHITE_PIXEL;
    }
  }

  return effect;
}
