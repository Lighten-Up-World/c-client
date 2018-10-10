#include "effect_runner.h"

volatile static server_args sa;

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
    {"1test", &get_test1_effect},
};

const string_to_constructor python[] = {
  {"python", &get_dummy_python_effect}
};

typedef struct {
  char *key;
  char *value;
} string_to_string;

const string_to_string cmds[] = {
  {"raverplaid", "python ../openpixelcontrol/python/raver_plaid.py &"},
  {"lavalamp", "python ../openpixelcontrol/python/lava_lamp.py  --layout layout/WorldMap.json &"},
  {"conway", "python ../openpixelcontrol/python/conway.py &"},
  {"snake", "python ../games/snake/snake.py &"}
};

const char *commands[] = {
  "temp_timelapse",
  "temp_log",
  "scroll",
  "image",
  "sun",
  "alltest",
  "1test",
  "raverplaid",
  "lavalamp",
  "conway"
};

void handle_user_exit(int _) {
  perror("Interrupted, cleaning up\n");
  sa.interrupted = 1;
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

effect_t *init_effect(const char *arg, void *pixel_info, opc_sink sink) {
  // Check for aliased commands
  for (int i = 0; i < sizeof(cmds) / sizeof(string_to_string); i++) {
    if (strcmp(cmds[i].key, arg) == 0) {
      // Execute OS command - THIS NEEDS KILLING BEFORE THE NEXT EFFECT RUNS
      system(cmds[i].value);
      puts("python returned");
      return python[0].new(pixel_info);
    }
  }

  effect_t *effect = NULL;
  for (int i = 0; i < sizeof(effects) / sizeof(string_to_constructor); i++) {
    if (strcmp(effects[i].str, arg) == 0) {
      effect = effects[i].new(pixel_info);
    }
  }
  if (effect == NULL) {
    fprintf(stderr, "Failed to construct effect using argument %s\n", arg);
    opc_close(sink);
    list_delete(pixel_info);
    exit(EXIT_FAILURE);
  }
  return effect;
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

// TODO: add generic cleanup code
int main() {
  // Set up Ctrl+C handle
  signal(SIGINT, handle_user_exit);

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

  // Initialise server args
  sa.shared_cmd = 0;
  sa.interrupted = 0;
  if (pthread_mutex_init((pthread_mutex_t *) &sa.mutex, NULL)) {
    perror("mutex create");
  }

  // Set up control server thread
  pthread_t server_thread_id;
  if (pthread_create(&server_thread_id,
                 NULL,
                 server,
                 (void *) &sa)) {
    perror("thread create");
  }

  // Set up basic server (for command line run args)
  // use the same args struct to write to the same shared command
  pthread_t basic_server_thread_id;
  if (pthread_create(&basic_server_thread_id,
                     NULL,
                     basic_server,
                     (void *) &sa)) {
    perror("thread create");
  }

  // Initialise the effect
  effect_t *effect = init_effect("scroll", pixel_info, sink);

  // Initialise api manager (should now never fail)
  effect_runner_t *effect_runner = effect_runner_init(NULL, effect, pixel_info, sink);

  // Clear Pixels
  CLEAR_PIXELS;

  // Run the effect
  while (!(sa.interrupted)) {
    nanosleep(&effect_runner->effect->time_delta, NULL);
    effect_runner->effect->run(effect_runner);
    effect_runner->frame_no++;

    // Handle server input
    pthread_mutex_lock((pthread_mutex_t *) &sa.mutex);
    if (sa.shared_cmd >= 0) {
      if (sa.shared_cmd < sizeof(commands) / sizeof(char *)) {
        puts("Cleaning up current effect...");
        // TODO: need to add code here to close effect.
        effect_runner->effect->remove(effect_runner->effect);

        printf("Running effect: %s\n", commands[sa.shared_cmd]);
        effect_runner->effect = init_effect(commands[sa.shared_cmd], pixel_info, sink);
        effect_runner->frame_no = 0;
        sa.shared_cmd = -1;
      }
    }
    pthread_mutex_unlock((pthread_mutex_t *) &sa.mutex);
  }

  // Close it all up
  effect_runner_delete(effect_runner);
  pthread_join(server_thread_id, NULL);

  return EXIT_SUCCESS;
}
