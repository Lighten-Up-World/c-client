#include "weather.h"

#define API_DELAY 5000
#define TIMELAPSE_DELAY 0.05

int weather_run(effect_runner_t *self){
  int i = self->frame_no % NUM_PIXELS;
  nanosleep(&self->effect->time_delta, NULL);
  self->effect->get_pixel(self, i);
  if(opc_put_pixel_list(self->sink, self->frame->pixels, self->pixel_info)) {
    return 1;
  }
  return 0;
}

int weather_timelapse_run(effect_runner_t *self){
  int i = self->frame_no;
  if (i >= NUM_PIXELS){
    i = 0;
    self->frame_no = 0;
    self->effect->get_pixel(self, i);
    if(!opc_put_pixel_list(self->sink, self->frame->pixels, self->pixel_info)) {
      return 1;
    }
  }else{
    self->effect->get_pixel(self, i);
  }
  nanosleep(&self->effect->time_delta, NULL);
  return 0;
}


int weather_log_run(effect_runner_t *self){
  int i = self->frame_no;
  if (i >= NUM_PIXELS){
    i = 0;
    self->frame_no = 0;
    sleep(1200);
  }
  nanosleep(&self->effect->time_delta, NULL);
  self->effect->get_pixel(self, i);
  return 0;
}

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
  size_t buf_size = 600;
  char buf[buf_size];
  if (get_data_for_geolocation(sockfd, &geoloc, WEATHER_HOST, WEATHER_PATH, WILL_OWM_API_KEY, buf, buf_size) < 0){
    return -1;
  }

  if(get_double_from_json(buf, attr, object, val) < 0){
    return -1;
  }

  socket_close(sockfd);

  return 0;
}

void set_windspeed_pixel_colour(opc_pixel_t *pixel, double val) {
  assert(pixel != NULL);
  printf("Windspeed: %f \n", val);
  pixel->b = PIXEL_COLOUR_MAX;

  int rg = (PIXEL_COLOUR_MAX / 10.0) * (val);
  rg = rg > PIXEL_COLOUR_MAX ? PIXEL_COLOUR_MAX : rg;
  pixel->g = rg;
  pixel->r = rg;
}

void set_temp_pixel_colour(opc_pixel_t *pixel, double val) {
  assert(pixel != NULL);
  val -= 273.0;
  int red = PIXEL_COLOUR_MIN;
  int blue = PIXEL_COLOUR_MIN;
  int green = PIXEL_COLOUR_MIN;

  //Red

  if (val > 0.0){
    red = (PIXEL_COLOUR_MAX / 15.0) * (val);
    red = red > PIXEL_COLOUR_MAX ? PIXEL_COLOUR_MAX : red;
  }

  //Green

  if (val > 15.0){
    green = (PIXEL_COLOUR_MAX / 30.0) * (40 - val);
    green = green < 0 ? 0 : green;
  }else if (val > -5){
    green = PIXEL_COLOUR_MAX;
  }else if (val > -10){
    green = (PIXEL_COLOUR_MAX / 10.0) * (-val);
  }

  //Blue
  if (val > -5.0){
    blue = (PIXEL_COLOUR_MAX / 10.0)  * (5-val);
    blue = blue < 0 ? 0 : blue;
  }else{
    blue = PIXEL_COLOUR_MAX;
  }

  // printf("r:%d, g:%d, b:%d \n", red, green, blue);

  pixel->b = blue;
  pixel->g = green;
  pixel->r = red;
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
  set_temp_pixel_colour(pixel, val);
  return 0;
}

int temp_get_pixel(effect_runner_t *self, int pos){
  return temp_get_pixel_for_xy(self->frame->pixels+pos, ((pixel_info_t *)list_get(self->pixel_info, pos))->geo);
}

effect_t *get_temp_effect(void * obj){
  effect_t *effect = calloc(1, sizeof(effect_t));
  if(effect == NULL){
    return NULL;
  }
  effect->get_pixel = &temp_get_pixel;
  effect->time_delta = (struct timespec){0, API_DELAY * MILLI_TO_NANO};
  effect->run = &weather_run;
  effect->remove = &free_effect;
  FILE *temp_file = fopen(TEMP_TIMELAPSE_WRITE_FILE, "a");
  if (temp_file == NULL){
    perror("temp_file2");
    exit(EXIT_FAILURE);
  }
  effect->obj = temp_file;
  return effect;
}

//LOGGING

int temp_log_pixel_for_xy(opc_pixel_t *pixel, geolocation_t geoloc, effect_t *eff, int loc) {
  assert(pixel != NULL);
  FILE *temp_file = eff->obj;
  double val;
  if (weather_get_val_for_xy(pixel, geoloc, "temp", "main", &val) < 0){
    return -1;
  }
  set_temp_pixel_colour(pixel, val);
  printf("%d %f\n", loc, val);
  fprintf(temp_file, "%d %f\n", loc, val);
  return 0;
}

int temp_log_pixel(effect_runner_t *self, int pos){
  return temp_log_pixel_for_xy(self->frame->pixels+pos, ((pixel_info_t *)list_get(self->pixel_info, pos))->geo, self->effect, pos);
}

effect_t *get_temp_log_effect(void *obj){
  effect_t *effect = calloc(1, sizeof(effect_t));
  if(effect == NULL){
    return NULL;
  }
  effect->get_pixel = &temp_log_pixel;
  effect->time_delta = (struct timespec){0, API_DELAY * MILLI_TO_NANO};
  effect->run = &weather_log_run;
  effect->remove = &free_file;
  FILE *temp_file = fopen(TEMP_TIMELAPSE_WRITE_FILE, "a");
  if (temp_file == NULL){
    perror("temp_file2");
    exit(EXIT_FAILURE);
  }
  effect->obj = temp_file;
  return effect;
}

//TIMELAPSE

int temp_timelapse_get_pixel(effect_runner_t *self, int pos){
  FILE *temp_file = self->effect->obj;
  char buffer[50];
  if (fgets(buffer, sizeof(buffer), temp_file) == NULL){
    fseek(temp_file, 0, SEEK_SET);
    fgets(buffer, sizeof(buffer), temp_file);
  }else{
    int loc = atoi(strtok(buffer, " "));
    double val = atof(strtok(NULL, " "));
    if (loc != pos){
      printf("LOC: %d, POS: %d", loc, pos);
      return -1;
    }
    set_temp_pixel_colour(self->frame->pixels+pos, val);
  }
  return 0;
}

effect_t *get_temp_timelapse_effect(void *obj) {
  effect_t *effect = calloc(1, sizeof(effect_t));
  if(effect == NULL){
    return NULL;
  }
  effect->get_pixel = &temp_timelapse_get_pixel;
  effect->time_delta = (struct timespec){0, TIMELAPSE_DELAY * MILLI_TO_NANO};
  effect->run = &weather_timelapse_run;
  effect->remove = &free_file;
  effect->get_frame = NULL;
  FILE *temp_file = fopen(TEMP_TIMELAPSE_READ_FILE, "r");
  if (temp_file == NULL){
    perror("temp_file");
    exit(EXIT_FAILURE);
  }
  effect->obj = temp_file;
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
  set_windspeed_pixel_colour(pixel, val);

  return 0;
}

int windspeed_get_pixel(effect_runner_t *self, int pos){
  return windspeed_get_pixel_for_xy(self->frame->pixels+pos, ((pixel_info_t *)list_get(self->pixel_info, pos))->geo);
}

effect_t *get_windspeed_effect(void *obj){
  effect_t *effect = calloc(1, sizeof(effect_t));
  if(effect == NULL){
    return NULL;
  }
  effect->get_pixel = &windspeed_get_pixel;
  effect->time_delta = (struct timespec){0, API_DELAY * MILLI_TO_NANO};
  effect->run = &weather_run;
  effect->remove = &free_effect;
  effect->get_frame = NULL;
  effect->obj = NULL;
  return effect;
}
