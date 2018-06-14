//
// Created by User on 14/06/2018.
//

#include "weather_api.h"

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

  geolocation_t geoloc = grid_geolocation(pixel->grid.x, pixel->grid.y);
  //TODO: FIX THIS SHIZ
  geoloc.latitude = rand() % 90;
  geoloc.longitude = rand() % 90;

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
  printf("%f \n", val);

  int red = 0;
  int blue = 0;
  int green = 0;

  if(val > 10){
    red = (255.0 / 10.0) * (val - 10.0);
    red = red > 255 ? 255.0 : red;
  }else{
    pixel->colour.red = 0;
  }

  if (val > 20){
    green = (255.0 / 10.0) * (30.0 - val);
    green = green < 0 ? 0 : green;
  }else if (val > 0){
    green = 255;
  }else if (val > -10){
    green = (255.0 / 10.0) * (10.0 + val);
  }else{
    green = 0;
  }

  if(val > 0){
    blue = (255.0 / 10.0) * (10.0 - val);
    blue = blue < 0 ? 0 : blue;
  }else{
    blue = 255;
  }

  pixel->colour.blue = blue;
  pixel->colour.green = green;
  pixel->colour.red = red;

  return 0;
}

//WINDSPEED

int windspeed_get_pixel_for_xy(pixel_t *pixel){
  assert(pixel != NULL);

  double val;
  if (weather_get_val_for_xy(pixel, "speed", "wind", &val) < 0){
    return -1;
  }

  if (val > 20.0){
    pixel->colour.red = 255;
    pixel->colour.green = 0;
    pixel->colour.blue = 0;
  }else if (val > 10.0){
    pixel->colour.red = 0;
    pixel->colour.green = 255;
    pixel->colour.blue = 0;
  }else{
    pixel->colour.red = 0;
    pixel->colour.green = 0;
    pixel->colour.blue = 255;
  }

  return 0;
}
