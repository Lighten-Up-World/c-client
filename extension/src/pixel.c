#include "pixel.h"
#include "extension.h"

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

typedef struct {
  int ret;
  int cnt;
  pixel_info_t key;
} search_pair_t;

void get_pos_enum(void *value, void *obj){
  pixel_info_t *pi = value;
  search_pair_t *sp = obj;
  if(pi->grid.x == sp->key.grid.x && pi->grid.y == sp->key.grid.y){
    sp->ret = sp->cnt;
  }
  else{
    sp->cnt++;
  }
}

int get_pos(int x, int y, list_t *pixel_info){
  search_pair_t sp = {-1, 0, {{x, y},{-1, -1}}};
  list_enum(pixel_info, &get_pos_enum, &sp);
  return sp.ret;
}
