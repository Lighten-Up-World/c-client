//
// Created by User on 14/06/2018.
//
#include "weather.h"

#define API_DELAY 5000
/*
int weather_run(effect_runner_t *self){
  // for (int i = 0; i < NUM_PIXELS; i++) {
  //   nanosleep(&self->effect->time_delta, NULL);
  //   self->effect->get_pixel(self, i, self->frame->pixels+i);
  //   if(!opc_put_pixels(self->socket, 0, NUM_PIXELS, self->frame->pixels)) {
  //     interrupted = 1;
  //     return -1;
  //   }
  // }
}
*/
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

int weather_get_val_for_xy(opc_pixel_t *pixel, geolocation_t geoloc, char *attr, char *object, double *val){
  assert(pixel != NULL);
  assert(attr != NULL);
  assert(object != NULL);

  int sockfd = socket_connect(WEATHER_HOST, 80);
  if (sockfd < 0){
    perror("SOCKFD");
    return -1;
  }

  printf("Latitude: %f, Longitude: %f, ", geoloc.latitude, geoloc.longitude);

  if (get_value_for_geolocation(sockfd,&geoloc, WEATHER_HOST, WEATHER_PATH, WILL_OWM_API_KEY, attr, object, val) < 0){
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

int temp_get_pixel_for_xy(opc_pixel_t *pixel, geolocation_t geoloc) {
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

int temp_get_pixel(effect_runner_t *self, int pos, opc_pixel_t *pixel){
  return temp_get_pixel_for_xy(pixel, ((pixel_info_t *)list_get(self->pixel_info, pos))->geo);
}

effect_t *get_temp_effect(void){
  effect_t *effect = calloc(1, sizeof(effect_t));
  if(effect == NULL){
    return NULL;
  }
  effect->get_pixel = &temp_get_pixel;
  effect->time_delta = (struct timespec){0, API_DELAY * MILLI_TO_NANO};
  return effect;
}

//// WINDSPEED ////

/***
 * WINDSPEED GET PIXEL FOR XY
 * Gets pixel data for the temperature mode.
 *
 * @param pixel - pixel to write values to.
 * @return 0 for success; -1 for failure
 */

int windspeed_get_pixel_for_xy(opc_pixel_t *pixel, geolocation_t geoloc){
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

int windspeed_get_pixel(effect_runner_t *self, int pos, opc_pixel_t *pixel){
  return windspeed_get_pixel_for_xy(pixel, ((pixel_info_t *)list_get(self->pixel_info, pos))->geo);
}

effect_t *get_windspeed_effect(void){
  effect_t *effect = calloc(1, sizeof(effect_t));
  if(effect == NULL){
    return NULL;
  }
  effect->get_pixel = &temp_get_pixel;
  effect->time_delta = (struct timespec){0, API_DELAY * MILLI_TO_NANO};
  return effect;
}
