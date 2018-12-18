#include <opc/opc_client.h>
#include "lasers.h"
#include "common.h"

int lasers_run(effect_runner_t *self) {

  // Move the laser head one pixel


  // Update the opc_pixel_t list
  for (int i = 0; i < NUM_PIXELS; i++) {
    self->frame->pixels[i] = (opc_pixel_t) {50, 50, 50};
  }
  self->frame->pixels[0] = WHITE_PIXEL;

  // Write the pixels to the display
  opc_put_pixel_list(self->sink, self->frame->pixels, self->pixel_info);

  return 0;
}

effect_t *get_lasers_effect(void *obj) {
  return NULL;
}

void free_test_effect2(effect_t *self) {
  opc_pixel_t **grid = self->obj;
  if(self != NULL){
    grid_free(grid);
  }
  free_effect(self);
}
