#include "common.h"

void free_effect(effect_t *self) {
  free(self);
}

void free_file(effect_t *self){
  if(self->obj){
    fclose(self->obj);
  }
  free_effect(self);
}
