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

int weather_get_val_for_xy(pixel_t *pixel, char *attr, char *object, double *val){
  assert(pixel != NULL);
  assert(attr != NULL);
  assert(object != NULL);

  int sockfd = socket_connect(WEATHER_HOST, 80);

  geolocation_t geoloc = grid2geolocation(pixel->grid.x, pixel->grid.y);

  if (get_value_for_geolocation(sockfd,&geoloc, WEATHER_HOST, WEATHER_PATH, attr, object, val) < 0){
    return -1;
  }

  return 0;
}

/***
 * TEMP GET PIXEL FOR XY
 * Gets pixel data for the temperature mode.
 *
 * @param pixel - pixel to write values to.
 * @return 0 for success; -1 for failure
 */

int temp_get_pixel_for_xy(pixel_t *pixel) {
  assert(pixel != NULL);

  double val;
  if (weather_get_val_for_xy(pixel, "temp", "main", &val) < 0){
    return -1;
  }
  val -= 273.0;

  int red = 0;
  int blue = 0;
  int green = 0;

  if(val > 10){
    red = (PIXEL_COLOUR_MAX / 10.0) * (val - 10.0);
    red = red > PIXEL_COLOUR_MAX ? PIXEL_COLOUR_MAX : red;
  }else{
    pixel->colour.red = 0;
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

  pixel->colour.blue = blue;
  pixel->colour.green = green;
  pixel->colour.red = red;

  return 0;
}

//// WINDSPEED ////

/***
 * WINDSPEED GET PIXEL FOR XY
 * Gets pixel data for the temperature mode.
 *
 * @param pixel - pixel to write values to.
 * @return 0 for success; -1 for failure
 */

int windspeed_get_pixel_for_xy(pixel_t *pixel){
  assert(pixel != NULL);

  double val;
  if (weather_get_val_for_xy(pixel, "speed", "wind", &val) < 0){
    return -1;
  }

  pixel->colour.blue = PIXEL_COLOUR_MAX;

  int rg = (PIXEL_COLOUR_MAX / 10.0) * (val);
  rg = rg > PIXEL_COLOUR_MAX ? PIXEL_COLOUR_MAX : rg;
  pixel->colour.green = rg;
  pixel->colour.red = rg;

  return 0;
}
