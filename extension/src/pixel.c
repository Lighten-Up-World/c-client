#include "pixel.h"

opc_pixel_t **grid_new(int cols, int rows) {
  opc_pixel_t **matrix;
  matrix = (opc_pixel_t **) malloc(cols * sizeof(opc_pixel_t *));
  if (!matrix) {
    return NULL;
  }

  matrix[0] = (opc_pixel_t *) malloc(rows * cols * sizeof(opc_pixel_t));
  if (!matrix[0]) {
    free(matrix);
    return NULL;
  }

  for (uint8_t i = 1; i < cols; i++) {
    matrix[i] = matrix[0] + i*rows;
  }
  return matrix;
}

opc_pixel_t **pixel_grid_new(void) {
  return grid_new(GRID_WIDTH, GRID_HEIGHT);
}

void grid_free(opc_pixel_t **pixel_grid) {
  free(pixel_grid[0]);
  free(pixel_grid);
}

int get_pos(int x, int y, list_t *pixel_info){
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixel_info_t *pi = list_get(pixel_info, i);
    if(pi->grid.x == x && pi->grid.y == y){
      return i;
    }
  }
  return -1;
}
