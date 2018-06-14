#include <zconf.h>
#include <printf.h>
#include <unistd.h>
#include "../opc.h"
#include "../opc_client.c"
#include "grid_to_opc.c"

// Refresh rate
#define ONE_MINUTE 60
#define TIME_DELTA 0.2
#define S_TO_MIRCO_SECONDS 10000

// Grid size
#define MAX_X 52
#define MAX_Y 24
#define NUM_PIXELS 471

#define CONFIG_FILE "layout/CoordsToListPos.txt"

volatile bool interrupted = false;

void handle_user_exit(int code) {
  interrupted = true;
}

// Run the simulation
int main(int argc, char** argv) {
  u8 channel = 0;
  u8 count;
  pixel pixels[NUM_PIXELS];

  // Open connection
  opc_sink sink = opc_new_sink("127.0.0.1:7890");

  signal(SIGINT, handle_user_exit);

  double time_elapsed;
  while (time_elapsed < ONE_MINUTE && !interrupted) {

    // Get the pixel data
    for (uint8_t x = 0; x < MAX_X; x++) {
      for (uint8_t y = 0; y < MAX_Y; y++) {
        // Get pixel at x, y
        //pixel p = {0,255,255};

        // Translate x, y into layout json position
        //int pos = get_pixel_location(x, y, CONFIG_FILE);
        //pixels[pos] = p;
      }
    }

    // Update the display
    opc_put_pixels(sink, channel, NUM_PIXELS, pixels);

    // Pause to slow down refresh rate
    usleep((useconds_t) (TIME_DELTA * S_TO_MIRCO_SECONDS * 100));
    time_elapsed += TIME_DELTA;
  }

  // Close the connection
  opc_close(sink);
  printf("cleanup\n");
}