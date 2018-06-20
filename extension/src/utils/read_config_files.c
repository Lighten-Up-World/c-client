#include <stdio.h>
#include <stdlib.h>
#include <utils/error.h>
#include <string.h>

 typedef struct {
   uint8_t channel;
   uint8_t position;
 } pixel_mapping_t;

/**
 * Get the channel number and position within the channel from an x, y coordinate, according to the configuration file
 * Configuration file should be of format: "x y channel pos"
 *
 * @param x: x location of pixel on map
 * @param y: y location of pixel on map
 * @param config_file: the file with configuration data
 * @return: the pixel number
 */
pixel_mapping_t get_led_data_from_grid(uint8_t x, uint8_t y, const char *config_file) {
  FILE *file = fopen(config_file, "r");
  if (file == NULL) {
    perror("File could not be opened");
    exit(EC_SYS);
  }

  size_t line_size = 100 * sizeof(char); //shouldn't be larger than this, should calculate and move to global constant
  char *buffer = malloc(line_size);
  while (fgets(buffer, (int) line_size, file) != NULL) {
    // Get x, y coordinate
    // If this is the coordinate searched for, return the pixel number
    if (atoi(strtok(buffer, " ")) == x) {
      if (atoi(strtok(NULL, " ")) == y) {
        pixel_mapping_t mapping;
        mapping.channel = (uint8_t) atoi(strtok(NULL, " "));
        mapping.position = (uint8_t) atoi(strtok(NULL, " "));
        return mapping;
      }
    }
  }

  if (ferror(file)) {
    perror("Failed to read from file");
    exit(EC_SYS);
  }

  if (fclose(file)) {
    perror("File could not be closed");
    exit(EC_SYS);
  }

  perror("No pixel found at grid coordinate given");
  exit(EC_INVALID_PARAM);
}

int get_channel_size(int channel, const char *channel_file) {
  FILE *file = fopen(channel_file, "r");
  if (file == NULL) {
    perror("File could not be opened");
    exit(EC_SYS);
  }

  size_t line_size = 100 * sizeof(char); //shouldn't be larger than this, should calculate and move to global constant
  char *buffer = malloc(line_size);
  int line = 0;
  while (fgets(buffer, (int) line_size, file) != NULL) {
    if (line == channel) {
      return atoi(buffer);
    }
  }

  if (ferror(file)) {
    perror("Failed to read from file");
    exit(EC_SYS);
  }

  if (fclose(file)) {
    perror("File could not be closed");
    exit(EC_SYS);
  }

  perror("No pixel found at grid coordinate given");
  exit(EC_INVALID_PARAM);
}