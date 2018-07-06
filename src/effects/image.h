#ifndef IMAGE_EFFECT_H
#define IMAGE_EFFECT_H

#include "../pixel.h"
#include "../extension.h"
#include "../opc/opc_client.h"
#include "../pixel.h"

int image_run(effect_runner_t* self);
effect_t *get_image_effect(void *obj);
void free_image(effect_t *self);

#endif
