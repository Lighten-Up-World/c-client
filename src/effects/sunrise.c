#include "sunrise.h"

// NOTE: All times requested by sunrise api are in UTC

void free_sunrise(effect_t *self){
  sun_state_t *state = self->obj;
  free(state->current_time);
  fclose(state->sun_file);
  free_effect(self);
}

void increment_current_time(effect_t *eff);

int sunrise_run(effect_runner_t *self){
  int i = self->frame_no;
  if (i >= NUM_PIXELS){
    i = 0;
    self->frame_no = 0;
    self->effect->get_pixel(self, i);
    increment_current_time(self->effect);
    if(!opc_put_pixel_list(self->sink, self->frame->pixels, self->pixel_info)) {
      return 1;
    }
  }else{
    self->effect->get_pixel(self, i);
  }
  nanosleep(&self->effect->time_delta, NULL);
  return 0;
}

int parse_time_string(char *time_chars, struct tm *time){
  assert(time_chars != NULL);
  assert(time != NULL);
  time->tm_hour = atoi(strtok(time_chars, ":"));
  time->tm_min = atoi(strtok(NULL, ":"));
  time->tm_sec = atoi(strtok(NULL, " "));

  if (strncmp(strtok(NULL, " "), "PM", 2) == 0){
    time->tm_hour += 12;
  }
  return 0;
}

double get_seconds_for_time(struct tm *time){
  return (3600 * time->tm_hour) + (60 * time->tm_min) + (time->tm_sec);
}

double get_time_difference(struct tm *first, struct tm *second){
  return get_seconds_for_time(first) - get_seconds_for_time(second);
}

void increment_current_time(effect_t *eff){
  sun_state_t *state = eff->obj;
  state->current_time->tm_min += 10;
  if (state->current_time->tm_min == 60){
    state->current_time->tm_min = 0;
    state->current_time->tm_hour += 1;
    if (state->current_time->tm_hour == 24){
      state->current_time->tm_hour = 0;
    }
  }
}


int sun_get_pixel(effect_runner_t *self, int pos){
  sun_state_t *state = self->effect->obj;
  size_t buf_size = 40;
  char buf[buf_size];
  struct tm sunrise;
  struct tm sunset;
  if(fgets(buf, buf_size, state->sun_file) == NULL){
    fseek(state->sun_file, 0, SEEK_SET);
    fgets(buf, buf_size, state->sun_file);
  }
  printf("%s \n", buf);
  int loc = atoi(strtok(buf, " "));
  sunrise.tm_hour = atoi(strtok(NULL, ":"));
  sunrise.tm_min = atoi(strtok(NULL, ":"));
  sunrise.tm_sec = atoi(strtok(NULL, " "));
  sunset.tm_hour = atoi(strtok(NULL, ":"));
  sunset.tm_min = atoi(strtok(NULL, ":"));
  sunset.tm_sec = atoi(strtok(NULL, " "));

  if (loc != pos){
    return -1;
  }

  //printf("SUNRISE IS: %d:%d:%d SUNSET IS: %d:%d:%d\n", sunrise.tm_hour, sunrise.tm_min, sunrise.tm_sec, sunset.tm_hour, sunset.tm_min, sunset.tm_sec);

  //printf("CURRENT: %d:%d:%d\n", state->current_time->tm_hour, state->current_time->tm_min, state->current_time->tm_sec);
  int set = PIXEL_COLOUR_MIN;

  double daylight = get_time_difference(&sunset, &sunrise);
  //printf("DAYLIGHT: %f\n", daylight);
  if (daylight == 0){
    set = 0;
  }else if (daylight > 0){
    //Then sunrise occurs before sunset
    double before_sunrise = get_time_difference(&sunrise, state->current_time);
    if (before_sunrise < 0){
      //Current is after sunrise
      double before_sunset = get_time_difference(&sunset, state->current_time);
      if (before_sunset > 0) {
        //Current is before sunset
        //printf("Set in greyscale: %d",set);
        set = (before_sunset / (daylight / 2.0)) * PIXEL_COLOUR_MAX;
        set = (set > PIXEL_COLOUR_MAX) ? (PIXEL_COLOUR_MAX) - set : set;
      }
    }
  }else{
    //Then sunset occurs before sunrise
    double before_sunset = get_time_difference(&sunset, state->current_time);
    if (before_sunset > 0){
      //Current is before sunset
      daylight += (3600 * 24);
      set = (before_sunset / (daylight / 2.0)) * PIXEL_COLOUR_MAX;
      set = (set > PIXEL_COLOUR_MAX) ? (PIXEL_COLOUR_MAX) - set : set;
      //printf("Set in greyscale: %d",set);
    }else{
      double before_sunrise = get_time_difference(&sunrise, state->current_time);
      if (before_sunrise < 0){
        //Current is after sunrise
        daylight += (3600 * 24);
        set = (-before_sunrise / (daylight / 2.0)) * PIXEL_COLOUR_MAX;
        set = (set > PIXEL_COLOUR_MAX) ? (PIXEL_COLOUR_MAX) - set : set;
        //printf("Set in greyscale: %d",set);
      }
    }
  }

  //printf("set: %d\n", set);

  opc_pixel_t *px = self->frame->pixels+pos;
  px->r = (uint8_t) set;
  px->b = (uint8_t) set;
  px->g = (uint8_t) set;


  return 0;
}

int load_sun_data(list_t *pixel_info){

  FILE *sun_file = fopen(SUN_FILE, "a");
  if (sun_file == NULL) {
    perror("sun_file");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < NUM_PIXELS; ++i) {

    int sockfd = socket_connect(SUNRISE_HOST, 80);
    if (sockfd < 0){
      perror("SOCKFD");
      return -1;
    }

    geolocation_t geoloc = ((pixel_info_t *)list_get(pixel_info, i))->geo;

    //printf("Latitude: %f, Longitude: %f, ", geoloc.latitude, geoloc.longitude);

    size_t buf_size = 600;
    char buff[buf_size];
    if (get_data_for_geolocation(sockfd, &geoloc, SUNRISE_HOST, SUNRISE_PATH, "0", buff, buf_size) < 0){
      return -1;
    }

    int json_start = 0;

    while (json_start < buf_size && buff[json_start] != '{'){
      json_start++;
    }

    char json_buff[buf_size - json_start];
    strncpy(json_buff, buff + json_start, buf_size - json_start);

    struct tm sunrise;
    struct tm sunset;

    char time[25];

    if(get_string_from_json(json_buff, "sunrise", "results", time) < 0){
      return -1;
    }

    parse_time_string(time, &sunrise);

    if(get_string_from_json(json_buff, "sunset", "results",  time) < 0){
      return -1;
    }

    parse_time_string(time, &sunset);

    //printf("%d\n", fprintf(sun_file, "%d %d:%d:%d %d:%d:%d\n", i, sunrise.tm_hour, sunrise.tm_min, sunrise.tm_sec, sunset.tm_hour, sunset.tm_min, sunset.tm_sec));

    socket_close(sockfd);
    sleep(1);
  }
  fclose(sun_file);
  return 0;
}


effect_t *get_sun_effect(void *obj) {
  //list_t *pixel_info = (list_t *) obj;
  effect_t *effect = calloc(1, sizeof(effect_t));
  sun_state_t *state = malloc(sizeof(sun_state_t));
  if (effect == NULL) {
    return NULL;
  }
  effect->get_pixel = &sun_get_pixel;
  effect->time_delta = (struct timespec) {0, 60000};
  effect->run = &sunrise_run;
  effect->remove = &free_sunrise;

  //load_sun_data(pixel_info);

  FILE *sun_file = fopen(SUN_FILE, "r");
  if (sun_file == NULL) {
    perror("sun_file");
    exit(EXIT_FAILURE);
  }
  /*
  size_t buf_size = 20;
  char buf[buf_size];

  time_t result = time(NULL);

  struct tm *current = localtime(&result);

  if (fgets(buf, buf_size, sun_file) == NULL) {
    load_sun_data(pixel_info, current);
  } else {
    int m = atoi(strtok(buf, " "));
    int y = atoi(strtok(NULL, " "));
    if (m != current->tm_mon || y != current->tm_year) {
      load_sun_data(pixel_info, current);
    }
  }
   */

  struct tm *curr = malloc(sizeof(struct tm));
  curr->tm_sec = 0;
  curr->tm_min = 0;
  curr->tm_hour = 0;
  state->current_time = curr;
  state->sun_file = sun_file;
  effect->obj = state;
  return effect;
}
