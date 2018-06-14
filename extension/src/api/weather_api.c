//
// Created by User on 14/06/2018.
//

#include "weather_api.h"

//TEMPERATURE

int temp_construct(api_manager_t *self){
  return tcp_construct(self, WEATHER_HOST);
}

int temp_destruct(api_manager_t *self){
  return tcp_destruct(self);
}

int temp_get_pixel_for_xy(api_manager_t *self, pixel_t *pixel, void *obj) {
  assert(obj != NULL);
  int *sock = self->obj;
  grid_t *grid = (grid_t *) obj;
  pixel->grid.x = grid->x;
  pixel->grid.y = grid->y;
  geolocation_t geoloc = grid_geolocation(pixel->grid.x, pixel->grid.y);
  double val;
  if (get_value_for_geolocation(*sock,&geoloc, WEATHER_HOST, WEATHER_PATH, "temp", &val) < 0){
    return -1;
  }
  pixel->colour.blue = val;
  pixel->colour.green = val;
  pixel->colour.red = val;
  return 0;
}

//WINDSPEED

int windspeed_construct(api_manager_t *self){
  return tcp_construct(self, WEATHER_HOST);

}

int windspeed_destruct(api_manager_t *self){
  return tcp_destruct(self);
}

int windspeed_get_pixel_for_xy(pixel_t *pixel, void *obj){
 /* grid_t *grid = (grid_t *) obj;
  pixel->grid = *grid;

  geolocation_t geoloc = grid_geolocation(pixel->grid.x, pixel->grid.y);

*/
  return 0;
}
