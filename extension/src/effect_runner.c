#include "effect_runner.h"

volatile int interrupted = 0;

typedef struct {
  char *str;
  api_t *(*new)(void);
} string_to_constructor;

const string_to_constructor apis[] = {
    {"temp", &get_temp_api},
    {"wind", &get_windspeed_api}
};

void handle_user_exit(int _) {
  interrupted = 1;
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
  opc_pixel_t pixels[NUM_PIXELS];

  signal(SIGINT, handle_user_exit);

  uint8_t channel = 0;
  opc_sink s;
  // Open connection
  s = opc_new_sink(HOST_AND_PORT);
  if(s == -1) {
    exit(EXIT_FAILURE);
  }

  //Clear Pixels
  for(int p = 0; p < NUM_PIXELS; p++) {
    pixels[p] = (opc_pixel_t) {PIXEL_COLOUR_MAX, PIXEL_COLOUR_MAX,PIXEL_COLOUR_MAX};
  }

  // Setup pixel_info
  list_t *pixel_info = list_new(&free);
  init_grid(pixel_info);
  init_geo(pixel_info);

  // API Call
  api_manager_t *api_manager= api_manager_new();
  api_t *api = NULL;

  for (int i = 0; i < sizeof(apis)/sizeof(string_to_constructor); i++) {
    if(strcmp(apis[i].str, argv[1]) == 0){
      api = apis[i].new();
    }
  }

  // Initialise api
  api_manager_init(api_manager, api, pixel_info);

  // Setup time delay
  struct timespec delay;
  delay.tv_sec = 0;
  delay.tv_nsec = 50 * MILLI_TO_NANO;

  while(!interrupted){
    for (int i = 0; i < NUM_PIXELS; i++) {
      if(interrupted){
        break;
      }
      nanosleep(&delay, NULL);
      api_manager->api->get_pixel(api_manager, i, pixels+i, NULL);
      if(!opc_put_pixels(s, channel, NUM_PIXELS, pixels)) {
        interrupted = 1;
        break;
      }
    }
  }

  // Close it all up
  api_manager_delete(api_manager);
  list_delete(pixel_info);
  opc_close(s);
  return 0;
}
