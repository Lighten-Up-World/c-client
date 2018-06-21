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
    pixels[i] = (opc_pixel_t) {0, 0, 255};
  }

  opc_put_pixels(sink, 0, NUM_PIXELS, pixels);
}
