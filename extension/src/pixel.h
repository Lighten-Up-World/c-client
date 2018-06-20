#ifndef PIXEL_H
#define PIXEL_H

#include "utils/list.h"
#include "extension.h"

opc_pixel_t **grid_new(int cols, int rows);
opc_pixel_t **pixel_grid_new(void);
void grid_free(opc_pixel_t **pixel_grid);
int get_pos(int x, int y, list_t *pixel_info);
void read_grid_to_list(opc_pixel_t **channel_pixels, opc_pixel_t **pixel_grid, list_t *pixel_info);

#endif
