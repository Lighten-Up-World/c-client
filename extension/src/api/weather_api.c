//
// Created by User on 14/06/2018.
//
#include "weather_api.h"

//// TEMPERATURE ////

/***
 * WEATHER GET VAL FOR XY
 * Performs common processing for the get_pixel functions.
 *
 * @param pixel - pixel to write grid values to.
 * @param grid - grid to write to the pixel
 * @param attr - name of attribute being retrieved
 * @param object - object that attribute is contained in
 * @param val - double pointer to write value to.
 * @return 0 for success; -1 for failure
 */

int weather_get_val_for_xy(pixel_t *pixel, geolocation_t geoloc, char *attr, char *object, double *val){
  assert(pixel != NULL);
  assert(attr != NULL);
  assert(object != NULL);

  int sockfd = socket_connect(WEATHER_HOST, 80);

  //Currently this function does not work
  //geolocation_t geoloc = grid2geolocation(pixel->grid.x, pixel->grid.y);

  //So generate random latitudes and longitudes to retrieve the data from
  //geoloc.latitude = rand() % 90;
  //geoloc.longitude = rand() % 90;

  printf("Latitude: %f, Longitude: %f,", geoloc.latitude, geoloc.longitude);

  if (get_value_for_geolocation(sockfd,&geoloc, WEATHER_HOST, WEATHER_PATH, DANIEL_OWM_API_KEY, attr, object, val) < 0){
    return -1;
  }

  socket_close(sockfd);

  return 0;
}

/***
 * TEMP GET PIXEL FOR XY
 * Gets pixel data for the temperature mode.
 *
 * @param pixel - pixel to write values to.
 * @return 0 for success; -1 for failure
 */

int temp_get_pixel_for_xy(pixel_t *pixel, geolocation_t geoloc) {
  assert(pixel != NULL);

  double val;
  if (weather_get_val_for_xy(pixel, geoloc, "temp", "main", &val) < 0){
    return -1;
  }
  val -= 273.0;

  printf("Temperature: %f \n", val);

  int red = 0;
  int blue = 0;
  int green = 0;

  if(val > 10){
    red = (PIXEL_COLOUR_MAX / 20.0) * (val - 10.0);
    red = red > PIXEL_COLOUR_MAX ? PIXEL_COLOUR_MAX : red;
  }else{
    pixel->r = 0;
  }

  if (val > 20){
    green = (PIXEL_COLOUR_MAX / 10.0) * (30.0 - val);
    green = green < 0 ? 0 : green;
  }else if (val > 0){
    green = PIXEL_COLOUR_MAX;
  }else if (val > -10){
    green = (PIXEL_COLOUR_MAX / 10.0) * (10.0 + val);
  }else{
    green = 0;
  }

  if(val > 0){
    blue = (PIXEL_COLOUR_MAX / 10.0) * (10.0 - val);
    blue = blue < 0 ? 0 : blue;
  }else{
    blue = PIXEL_COLOUR_MAX;
  }

  pixel->b = blue;
  pixel->g = green;
  pixel->r = red;

  return 0;
}

int temp_get_pixel(api_manager_t *self, int pos, pixel_t *pixel, void *obj){
  return temp_get_pixel_for_xy(pixel, ((pixel_info_t *)list_get(self->pixel_info, pos))->geo);
}

api_t *get_temp_api(void){
  api_t *api = malloc(sizeof(api_t));
  if(api == NULL){
    return NULL;
  }
  api->name = "temp";
  api->get_pixel = &temp_get_pixel;
  return api;
}

//// WINDSPEED ////

/***
 * WINDSPEED GET PIXEL FOR XY
 * Gets pixel data for the temperature mode.
 *
 * @param pixel - pixel to write values to.
 * @return 0 for success; -1 for failure
 */

int windspeed_get_pixel_for_xy(pixel_t *pixel, geolocation_t geoloc){
  assert(pixel != NULL);

  double val;
  if (weather_get_val_for_xy(pixel, geoloc, "speed", "wind", &val) < 0){
    return -1;
  }

  printf("Windspeed: %f \n", val);
  pixel->b = PIXEL_COLOUR_MAX;

  int rg = (PIXEL_COLOUR_MAX / 10.0) * (val);
  rg = rg > PIXEL_COLOUR_MAX ? PIXEL_COLOUR_MAX : rg;
  pixel->g = rg;
  pixel->r = rg;

  return 0;
}

int windspeed_get_pixel(api_manager_t *self, int pos, pixel_t *pixel, void *obj){
  return windspeed_get_pixel_for_xy(pixel, ((pixel_info_t *)list_get(self->pixel_info, pos))->geo);
}

api_t *get_windspeed_api(void){
  api_t *api = malloc(sizeof(api_t));
  if(api == NULL){
    return NULL;
  }
  api->name = "windspeed";
  api->get_pixel = &temp_get_pixel;
  return api;
}
