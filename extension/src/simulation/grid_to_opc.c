#include <stdlib.h>
#include "utils/error.h"
#include <string.h>
#include "grid_to_opc.h"
#include "utils/io.h"

/**
 * Get the number of lines in a file
 *
 * @param path: path to the file to read from
 * @return: the number of lines in the file (including trailing whitespace lines)
 */
int get_num_lines(const char *path) {
  FILE *file = fopen(path, "r");
  if (file == NULL) {
    perror("File could not be opened");
    exit(EC_SYS);
  }

  size_t line_size = 100 * sizeof(char); //shouldn't be larger than this, should calculate and move to global constant
  char *buffer = malloc(line_size);
  int line = 0;
  while (fgets(buffer, (int) line_size, file) != NULL) {
    line++;
  }

  if (ferror(file)) {
    free(buffer);
    perror("Failed to read from file");
    exit(EC_SYS);
  }

  if (fclose(file)) {
    free(buffer);
    perror("File could not be closed");
    exit(EC_SYS);
  }

  free(buffer);
  return line;
}

/**
 * Get the pixel number from an x, y coordinate, according to the configuration file
 * Configuration file should be of format: x y #\n
 *
 * @param x: x location of pixel on map
 * @param y: y location of pixel on map
 * @param config_file: the file with configuration data
 * @return: the pixel number
 */
int get_pixel_location(uint8_t x, uint8_t y, const char *config_file) {
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
        return atoi(strtok(NULL, " "));
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