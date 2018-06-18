#include "effect_runner.h"

volatile int interrupted = 0;

typedef struct {
  char *str;
  effect_t *(*new)(void);
} string_to_constructor;

const string_to_constructor effects[] = {
    {"temp", &get_temp_effect},
    {"windspeed", &get_windspeed_effect}
};

void handle_user_exit(int _) {
  interrupted = 1;
}

effect_runner_t *effect_runner_new(void){
  effect_runner_t *effect_runner = NULL;
  effect_runner = malloc(sizeof(effect_runner_t));
  if (effect_runner == NULL){
    perror("effect_runner_new failed");
    return NULL;
  }
  effect_runner->effect = NULL;
  effect_runner->pixel_info = NULL;
  effect_runner->frame = calloc(1, sizeof(frame_t));
  return effect_runner;
}

effect_runner_t *effect_runner_init(effect_runner_t *self, effect_t *effect, list_t *pixel_info){
  if(self == NULL) {
    self = effect_runner_new();
  }
  if(self != NULL){
    self->effect = effect;
    self->pixel_info = pixel_info;
  }
  return self;
}

void effect_runner_delete(effect_runner_t *self) {
  free(self);
  if(self != NULL){
    list_delete(self->pixel_info);
    free(self->effect);
    free(self->frame);
  }
}


int init_grid(list_t* list){
  csv_parser_t *coords_to_pos_parser = csv_parser_new(PIXEL_FILE, " ");
  csv_row_t *row;
  while ((row = csv_parser_getRow(coords_to_pos_parser)) ) {
      char **rowFields = csv_parser_getFields(row);
      int pos = atoi(rowFields[2]);
      pixel_info_t *pi;
      if((pi = list_get(list, pos)) == NULL){
        pi = malloc(sizeof(pixel_info_t));
        *pi = (pixel_info_t){{.x = atoi(rowFields[0]), .y =atoi(rowFields[1])},
                {-1, -1}};
        list_add(list, pi); //TODO: NEEDS TO ADD IN CORRECT POS
      }
      else {
        pi->grid = (grid_t){.x = atoi(rowFields[0]), .y =atoi(rowFields[1])};
      }
      csv_parser_destroy_row(row);
  }
  csv_parser_destroy(coords_to_pos_parser);
  return 0;
}

int init_geo(list_t* list){
  csv_parser_t *coords_to_pos_parser = csv_parser_new(GEOLOC_FILE, " ");
  csv_row_t *row;
  while ((row = csv_parser_getRow(coords_to_pos_parser)) ) {
      char **rowFields = csv_parser_getFields(row);

      int pos = atoi(rowFields[2]);
      pixel_info_t *pi;
      if((pi = list_get(list, pos)) == NULL){
        pi = malloc(sizeof(pixel_info_t));
        *pi = (pixel_info_t){{-1, -1},
                {.latitude = atof(rowFields[0]), .longitude =atof(rowFields[1])}};
        list_add(list, pi); //TODO: NEEDS TO ADD IN CORRECT POS
      }
      else {
        pi->geo = (geolocation_t){.latitude = atof(rowFields[0]), .longitude =atof(rowFields[1])};
      }

      csv_parser_destroy_row(row);
  }
  csv_parser_destroy(coords_to_pos_parser);
  return 0;
}

int main(int argc, const char * argv[]) {
  assert(argc > 1);

  signal(SIGINT, handle_user_exit);

  uint8_t channel = 0;
  opc_sink s;
  // Open connection
  s = opc_new_sink(HOST_AND_PORT);
  if(s == -1) {
    exit(EXIT_FAILURE);
  }

  // Setup pixel_info
  list_t *pixel_info = list_new(&free);
  if(pixel_info == NULL){
    opc_close(s);
    exit(EXIT_FAILURE);
  }
  init_grid(pixel_info);
  init_geo(pixel_info);

  // Find effect from argument
  effect_t *effect = NULL;
  for (int i = 0; i < sizeof(effects)/sizeof(string_to_constructor); i++) {
    if(strcmp(effects[i].str, argv[1]) == 0){
      effect = effects[i].new();
    }
  }
  if(effect == NULL){
    fprintf(stderr, "Failed to construct effect using argument %s\n", argv[1]);
    opc_close(s);
    list_delete(pixel_info);
    exit(EXIT_FAILURE);
  }

  // Initialise api manager
  effect_runner_t *effect_runner = effect_runner_init(NULL, effect, pixel_info);
  if(effect_runner == NULL){
    fprintf(stderr, "Api_manager failed to initialise with effect %s\n", argv[1]);
    free(effect);
    opc_close(s);
    list_delete(pixel_info);
    exit(EXIT_FAILURE);
  }

  //Clear Pixels
  for(int p = 0; p < NUM_PIXELS; p++) {
    effect_runner->frame->pixels[p] = (opc_pixel_t) {PIXEL_COLOUR_MAX, PIXEL_COLOUR_MAX,PIXEL_COLOUR_MAX};
  }

  while(!interrupted){
    for (int i = 0; i < NUM_PIXELS; i++) {
      if(interrupted){
        break;
      }
      nanosleep(&effect_runner->effect->time_delta, NULL);
      effect_runner->effect->get_pixel(effect_runner, i, effect_runner->frame->pixels+i);
      if(!opc_put_pixels(s, channel, NUM_PIXELS, effect_runner->frame->pixels)) {
        interrupted = 1;
        break;
      }
    }
  }

  // Close it all up
  effect_runner_delete(effect_runner);
  opc_close(s);
  return EXIT_SUCCESS;
}
