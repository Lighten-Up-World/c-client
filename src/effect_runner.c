#include "effect_runner.h"

volatile int interrupted = 0;

typedef struct {
  char *str;
  effect_t *(*new)(void *);
} string_to_constructor;

const string_to_constructor effects[] = {
    {"temp", &get_temp_effect},
    {"temp_timelapse", &get_temp_timelapse_effect},
    {"temp_log", &get_temp_log_effect},
    {"windspeed", &get_windspeed_effect},
    {"scroll", &get_scroller_effect},
    {"temp_timelapse", &get_temp_timelapse_effect},
    {"image", &get_image_effect},
    {"sun", &get_sun_effect},
    {"alltest", &get_alltest_effect},
    {"1test", &get_test1_effect}
};

typedef struct {
  char *key;
  char *value;
} string_to_string;

const string_to_string cmds[] = {
  {"raverplaid", "python ../openpixelcontrol/python/raver_plaid.py"},
  {"lavalamp", "python ../openpixelcontrol/python/lava_lamp.py  --layout ../layout/WorldMap.json"},
  {"conway", "python ../openpixelcontrol/python/conway.py"},
  {"snake", "python ../games/snake/snake.py"}
};

void handle_user_exit(int _) {
  perror("Interrupted, cleaning up\n");
  interrupted = 1;
}

effect_runner_t *effect_runner_new(void) {
  effect_runner_t *effect_runner = NULL;
  effect_runner = malloc(sizeof(effect_runner_t));
  if (effect_runner == NULL) {
    perror("effect_runner_new failed");
    return NULL;
  }
  effect_runner->effect = NULL;
  effect_runner->pixel_info = NULL;
  effect_runner->sink = -1;
  effect_runner->frame_no = 0;
  effect_runner->frame = calloc(1, sizeof(frame_t));
  return effect_runner;
}

effect_runner_t *effect_runner_init(effect_runner_t *self, effect_t *effect, list_t *pixel_info, opc_sink sink) {
  if (self == NULL) {
    self = effect_runner_new();
  }
  if (self != NULL) {
    self->effect = effect;
    self->pixel_info = pixel_info;
    self->sink = sink;
  }
  return self;
}

void effect_runner_delete(effect_runner_t *self) {
  if (self != NULL) {
    opc_close(self->sink);
    list_delete(self->pixel_info);
    self->effect->remove(self->effect);
    free(self->frame);
  }
  free(self);
}

int init_grid(list_t *list) {
  csv_parser_t *coords_to_pos_parser = csv_parser_new(PIXEL_FILE, " ");
  csv_row_t *row;
  while ((row = csv_parser_getRow(coords_to_pos_parser))) {
    char **rowFields = csv_parser_getFields(row);
    int pos = atoi(rowFields[2]);
    pixel_info_t *pi;
    if ((pi = list_get(list, pos)) == NULL) {
      pi = malloc(sizeof(pixel_info_t));
      *pi = (pixel_info_t) {{.x = atoi(rowFields[0]), .y =atoi(rowFields[1])},
          .geo={-1, -1}, .strip={-1, -1}};
      list_add(list, pi); //TODO: NEEDS TO ADD IN CORRECT POS
    } else {
      pi->grid = (grid_t) {.x = atoi(rowFields[0]), .y =atoi(rowFields[1])};
    }
    csv_parser_destroy_row(row);
  }
  csv_parser_destroy(coords_to_pos_parser);
  return 0;
}

int init_geo(list_t *list) {
  csv_parser_t *coords_to_pos_parser = csv_parser_new(GEOLOC_FILE, " ");
  csv_row_t *row;
  while ((row = csv_parser_getRow(coords_to_pos_parser))) {
    char **rowFields = csv_parser_getFields(row);

    int pos = atoi(rowFields[2]);
    pixel_info_t *pi;
    if ((pi = list_get(list, pos)) == NULL) {
      perror("Pixel Info list should already by populated by init_grid");
      return 1;
    } else {
      pi->geo = (geolocation_t) {.latitude = atof(rowFields[0]), .longitude = atof(rowFields[1])};
    }
    //free(rowFields);
    csv_parser_destroy_row(row);
  }
  csv_parser_destroy(coords_to_pos_parser);
  return 0;
}

int init_strip(list_t *list) {
  csv_parser_t *parser = csv_parser_new(STRIP_FILE, " ");
  csv_row_t *row;
  while ((row = csv_parser_getRow(parser))) {
    char **rowFields = csv_parser_getFields(row);

    int pos = atoi(rowFields[2]);
    pixel_info_t *pi;
    if ((pi = list_get(list, pos)) == NULL) {
      perror("Pixel Info list should already by populated by init_grid");
      return 1;
    } else {
      pi->strip = (strip_t) {.channel = atoi(rowFields[0]), .num = atoi(rowFields[1])};
    }
    //free(rowFields);
    csv_parser_destroy_row(row);
  }
  csv_parser_destroy(parser);
  return 0;
}

int server_has_input(int server) {
  return 1;
}

int run_effect(const char *effect_arg) {

  // Check for aliased commands
  for (int i = 0; i < sizeof(cmds) / sizeof(string_to_string); i++) {
    if (strcmp(cmds[i].key, effect_arg) == 0) {
      return system(cmds[i].value);
    }
  }

  // Open connection
  opc_sink sink = opc_new_sink(HOST_AND_PORT);
  if (sink == -1) {
    exit(EXIT_FAILURE);
  }

  // Setup pixel_info
  list_t *pixel_info = list_new(&free);
  if (pixel_info == NULL) {
    opc_close(sink);
    exit(EXIT_FAILURE);
  }
  init_grid(pixel_info);
  init_geo(pixel_info);
  init_strip(pixel_info);

  // Find effect from argument
  effect_t *effect = NULL;
  for (int i = 0; i < sizeof(effects) / sizeof(string_to_constructor); i++) {
    if (strncmp(effects[i].str, effect_arg, strlen(effects[i].str)) == 0) {
      effect = effects[i].new(pixel_info);
    }
  }
  if (effect == NULL) {
    fprintf(stderr, "Failed to construct effect using argument %s\n", effect_arg);
    opc_close(sink);
    list_delete(pixel_info);
    exit(EXIT_FAILURE);
  }

  // Initialise api manager
  effect_runner_t *effect_runner = effect_runner_init(NULL, effect, pixel_info, sink);
  if (effect_runner == NULL) {
    fprintf(stderr, "Api_manager failed to initialise with effect %s\n", effect_arg);
    free(effect);
    opc_close(sink);
    list_delete(pixel_info);
    exit(EXIT_FAILURE);
  }

  // Clear Pixels
  for (int p = 0; p < NUM_PIXELS; p++) {
    effect_runner->frame->pixels[p] = WHITE_PIXEL;
  }

  // Run the effect
  while (!interrupted) {
    nanosleep(&effect_runner->effect->time_delta, NULL);
    effect_runner->effect->run(effect_runner);
    effect_runner->frame_no++;
  }

  // Close it all up
  effect_runner_delete(effect_runner);
  return EXIT_SUCCESS;
}

int web_server(void) {
  // Create listening socket, setup server
  ctrl_server *server = start_server();
  char *read;
  printf("Server started on %s\n", LISTEN_PORT);
  // Simulates main loop of effect runner
  while (!interrupted) {

    // *** Effect runner code here ***
    //printf("Looking for a client with %i\n", server->client_fd);
    // If client is connected
    if (server->client_fd) {
      read = read_ws_frame(server);
      run_effect(read);
      free(read);
    } else {
      // Check if any client is waiting to connect
      server->client_fd = try_accept_conn(server);

      // If a connection was accepted, block until it's upgraded it to a WebSocket
      if (server->client_fd) {
        ssize_t read_size;
        int tries = 0;
        char *http_buffer = calloc(HTTP_BUFFER + 1, sizeof(char));
        do {
          read_size = get_latest_input(server, http_buffer, HTTP_BUFFER);
          tries++;
        } while (read_size < 0 && tries < 5);

        if (read_size > 0) {
          puts("Upgrading to WebSocket...");
          if (try_to_upgrade(server, http_buffer)) {
            close_client(server);
          }
        } else {
          close_client(server);
        }

        free(http_buffer);
      }

    }
    sleep_for(1);
  }

  close_server(server);

  return EXIT_SUCCESS;
}

int main(int argc, const char *argv[]) {
  assert(argc > 1);

  // Set up Ctrl+C handle
  signal(SIGINT, handle_user_exit);

  if (strcmp(argv[1], "-ws") == 0) {
    web_server();
  }

  run_effect(argv[1]);
}
