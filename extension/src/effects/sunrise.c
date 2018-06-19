#include "sunrise.h"

// NOTE: All times requested by sunrise api are in UTC

void increment_current_time(effect_t *eff);

int sunrise_run(effect_runner_t *self){

  int i = self->frame_no % NUM_PIXELS;
  nanosleep(&self->effect->time_delta, NULL);
  self->effect->get_pixel(self, i);
  if(!opc_put_pixels(self->sink, 0, NUM_PIXELS, self->frame->pixels)) {
    return 1;
  }
  increment_current_time(self->effect);
  return 0;
}

int parse_time_string(char *time_chars, struct tm *time){
  assert(time_chars != NULL);
  assert(time != NULL);
  time->tm_hour = atoi(strtok(time_chars, ":"));
  time->tm_min = atoi(strtok(NULL, ":"));
  time->tm_sec = atoi(strtok(NULL, " "));
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
  state->current_time->tm_sec += 10;
  if (state->current_time->tm_sec == 60){
    state->current_time->tm_sec = 0;
    state->current_time->tm_min += 1;
    if (state->current_time->tm_min == 60){
      state->current_time->tm_min = 0;
      state->current_time->tm_hour += 1;
      if (state->current_time->tm_hour == 24){
        state->current_time->tm_hour = 0;
      }
    }
  }
}


int sun_get_pixel(effect_runner_t *self, int pos){
  /*
  sun_state_t *state = self->effect->obj;
  size_t buf_size = 40;
  char buf[buf_size];
  struct tm sunrise;
  struct tm sunset;
  if(fgets(buf, buf_size, state->sun_file) == NULL){
    return -1;
  }
  int loc = atoi(strtok(buf, " "));
  sunrise.tm_hour = atoi(strtok(buf, ":"));
  sunrise.tm_min = atoi(strtok(NULL, ":"));
  sunrise.tm_sec = atoi(strtok(NULL, " "));

  sunset.tm_hour = atoi(strtok(buf, ":"));
  sunset.tm_min = atoi(strtok(NULL, ":"));
  sunset.tm_sec = atoi(strtok(NULL, " "));

  if (loc == pos){
    return -1;
  }

  uint8_t r,g,b;

  double seconds_to_solar_noon = get_time_difference(sunset, sunrise) / 2.0;
  printf("daylight seconds: %f", daylight);

  double fromsunrise = get_time_difference(state->current_time, sunrise);
  if (fromsunrise < 0){
    r = 0;
    g = 0;
    b = 0;
  }

  opc_pixel_t *px = self->frame->pixels+pos;
  px->r = r;
  px->b = b;
  px->g = g;

   */
  return 0;
}

int load_sun_data(list_t *pixel_info, FILE *sun_file, struct tm *current){
  assert(sun_file != NULL);

  fprintf(sun_file, "%d %d", current->tm_mon, current->tm_year);
  for (int i = 0; i < NUM_PIXELS; ++i) {

    int sockfd = socket_connect(SUNRISE_HOST, 80);
    if (sockfd < 0){
      perror("SOCKFD");
      return -1;
    }

    geolocation_t geoloc = ((pixel_info_t *)list_get(pixel_info, i))->geo;

    printf("Latitude: %f, Longitude: %f, ", geoloc.latitude, geoloc.longitude);

    size_t buf_size = 600;
    char buff[buf_size];
    if (get_data_for_geolocation(sockfd, &geoloc, SUNRISE_HOST, SUNRISE_PATH, "0", buff, buf_size) < 0){
      return -1;
    }

    struct tm sunrise;
    struct tm sunset;

    char time[25];

    if(get_string_from_json(buff, "sunrise", "result", time) < 0){
      return -1;
    }

    parse_time_string(time, &sunrise);

    if(get_string_from_json(buff, "sunset", "result",  time) < 0){
      return -1;
    }

    parse_time_string(time, &sunset);
    sunset.tm_hour += 12;

    fprintf(sun_file, "%d %d:%d:%d %d:%d:%d\n", i, sunrise.tm_hour, sunrise.tm_min, sunrise.tm_sec, sunset.tm_hour, sunset.tm_min, sunset.tm_sec);

    socket_close(sockfd);
    sleep(1);
  }
  return 0;
}


effect_t *get_sun_effect(void *obj) {
  list_t *pixel_info = (list_t *) obj;
  effect_t *effect = calloc(1, sizeof(effect_t));
  sun_state_t *state = malloc(sizeof(sun_state_t));
  if (effect == NULL) {
    return NULL;
  }
  effect->get_pixel = &sun_get_pixel;
  effect->time_delta = (struct timespec) {1, 0};
  effect->run = &sunrise_run;
  FILE *sun_file = fopen(SUN_FILE, "a");
  if (sun_file == NULL) {
    perror("sun_file");
    exit(EXIT_FAILURE);
  }
  size_t buf_size = 20;
  char buf[buf_size];

  time_t result = time(NULL);

  struct tm *current = localtime(&result);

  if (fgets(buf, buf_size, sun_file) == NULL) {
    load_sun_data(pixel_info, sun_file, current);
  } else {
    int m = atoi(strtok(buf, " "));
    int y = atoi(strtok(NULL, " "));
    if (m != current->tm_mon || y != current->tm_year) {
      load_sun_data(pixel_info, sun_file, current);
    }
  }
  struct tm *curr = malloc(sizeof(curr));
  curr->tm_sec = 0;
  curr->tm_min = 0;
  curr->tm_hour = 0;
  state->current_time = curr;
  state->sun_file = sun_file;
  effect->obj = state;
  return effect;
}
