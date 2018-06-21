#include <signal.h>
#include "opc/opc_client.h"

#define DIM_PIXEL (opc_pixel_t) {50,50,50}

int main() {
  opc_sink sink;

  // Open connection
  sink = opc_new_sink(HOST_AND_PORT);
  if (sink == -1) {
    exit(EXIT_FAILURE);
  }

  opc_pixel_t *pixels = calloc(NUM_PIXELS, sizeof(opc_pixel_t));
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels[i] = BLACK_PIXEL;
  }

  opc_put_pixels(sink, 0, NUM_PIXELS, pixels);

  struct timespec ts;
  int milli = 1;
  //ts.tv_sec = milli / 1000;
  ts.tv_nsec = milli * 1000000;

  for (int x = 0; x < NUM_PIXELS; x++) {
    pixels[x] = WHITE_PIXEL;
    opc_put_pixels(sink, 0, NUM_PIXELS, pixels);
    nanosleep(&ts, NULL);
  }

}
