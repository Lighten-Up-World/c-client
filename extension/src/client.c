/* Copyright 2013 Ka-Ping Yee

Licensed under the Apache License, Version 2.0 (the "License"); you may not
use this file except in compliance with the License.  You may obtain a copy
of the License at: http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.  See the License for the
specific language governing permissions and limitations under the License. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zconf.h>
#include "../../src/utils/error.h"
#include "opc/opc.h"
#include "opc/opc_client.c"

#define MAX_PIXELS (65535/3)
#define MAX_INPUT_LENGTH (MAX_PIXELS*8)

int main(int argc, char** argv) {
  u8 channel = 0;
  u16 count = 10;
  opc_sink s;

  // Open connection
  s = opc_new_sink("127.0.0.1:7890");

  // Set pixels
  pixel pixels[10];
  for (int i = 0; i < 10; i++) {
    pixels[i] = (pixel) {0,0,255};
  }

  // s: socket
  // channel: 0 is all channels (this defines the LED strip we access - doesn't matter for simulation?)
  // count: number of pixels to set
  // pixels: the list of pixels to set
  u8 ret = opc_put_pixels(s, channel, count, pixels);
  printf("pixels sent code (1 if all data sent): %d\n", ret);

  sleep(2);
}
