/* Copyright 2013 Ka-Ping Yee

Licensed under the Apache License, Version 2.0 (the "License"); you may not
use this file except in compliance with the License.  You may obtain a copy
of the License at: http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.  See the License for the
specific language governing permissions and limitations under the License.

  Modified by arm11_22 team
 */

#include "opc_client.h"

static opc_sink_info opc_sinks[OPC_MAX_SINKS];
static opc_sink opc_next_sink = 0;

int opc_resolve(char  *s, struct sockaddr_in* address, uint16_t default_port) {
  struct addrinfo* addr;
  struct addrinfo* ai;
  long port = 0;
  char* name = strdup(s);
  char* colon = strchr(name, ':');

  if (colon) {
    *colon = 0;
    port = strtol(colon + 1, NULL, 10);
  }
  getaddrinfo(colon == name ? "localhost" : name, NULL, NULL, &addr);
  free(name);
  for (ai = addr; ai; ai = ai->ai_next) {
    if (ai->ai_family == PF_INET) {
      memcpy(address, addr->ai_addr, sizeof(struct sockaddr_in));
      address->sin_port = htons(port ? port : default_port);
      freeaddrinfo(addr);
      return 1;
    }
  }
  freeaddrinfo(addr);
  return 0;
}

opc_sink opc_new_sink(char* hostport) {
  opc_sink_info* info;

  /* Allocate an opc_sink_info entry. */
  if (opc_next_sink >= OPC_MAX_SINKS) {
    fprintf(stderr, "OPC: No more sinks available\n");
    return -1;
  }
  info = &opc_sinks[opc_next_sink];

  /* Resolve the server address. */
  info->sockid = -1;
  if (!opc_resolve(hostport, &(info->address), OPC_DEFAULT_PORT)) {
    fprintf(stderr, "OPC: Host not found: %s\n", hostport);
    return -1;
  }
  inet_ntop(AF_INET, &(info->address.sin_addr), info->address_string, 64);
  sprintf(info->address_string + strlen(info->address_string),
          ":%d", ntohs(info->address.sin_port));

  /* Increment opc_next_sink only if we were successful. */
  return opc_next_sink++;
}

/* Makes one attempt to open the connection for a sink if needed, timing out */
/* after timeout_ms.  Returns 1 if connected, 0 if the timeout expired. */
static uint8_t opc_connect(opc_sink sink, uint32_t timeout_ms) {
  int sock;
  struct timeval timeout;
  opc_sink_info* info = &opc_sinks[sink];
  fd_set writefds;
  int opt_errno;
  socklen_t len;

  if (sink < 0 || sink >= opc_next_sink) {
    fprintf(stderr, "OPC: Sink %d does not exist\n", sink);
    return 0;
  }
  if (info->sockid >= 0) {  /* already connected */
    return 1;
  }

  /* Do a non-blocking connect so we can control the timeout. */
  sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  fcntl(sock, F_SETFL, O_NONBLOCK);
  if (connect(sock, (struct sockaddr*) &(info->address),
              sizeof(info->address)) < 0 && errno != EINPROGRESS) {
    fprintf(stderr, "OPC: Failed to connect to %s: ", info->address_string);
    perror(NULL);
    close(sock);
    return 0;
  }

  /* Wait for a result. */
  FD_ZERO(&writefds);
  FD_SET(sock, &writefds);
  timeout.tv_sec = timeout_ms/1000;
  timeout.tv_usec = timeout_ms % 1000;
  select(sock + 1, NULL, &writefds, NULL, &timeout);
  if (FD_ISSET(sock, &writefds)) {
    opt_errno = 0;
    getsockopt(sock, SOL_SOCKET, SO_ERROR, &opt_errno, &len);
    if (opt_errno == 0) {
      fprintf(stderr, "OPC: Connected to %s\n", info->address_string);
      info->sockid = sock;
      return 1;
    } else {
      fprintf(stderr, "OPC: Failed to connect to %s: %s\n",
              info->address_string, strerror(opt_errno));
      close(sock);
      if (opt_errno == ECONNREFUSED) {
        usleep(timeout_ms*1000);
      }
      return 0;
    }
  }
  fprintf(stderr, "OPC: No connection to %s after %d ms\n",
          info->address_string, timeout_ms);
  return 0;
}

/* Closes the connection for a sink. */
void opc_close(opc_sink sink) {
  opc_sink_info* info = &opc_sinks[sink];

  if (sink < 0 || sink >= opc_next_sink) {
    fprintf(stderr, "OPC: Sink %d does not exist\n", sink);
    return;
  }
  if (info->sockid >= 0) {
    close(info->sockid);
    info->sockid = -1;
    fprintf(stderr, "OPC: Closed connection to %s\n", info->address_string);
  }
}

/* Sends data to a sink, making at most one attempt to open the connection */
/* if needed and waiting at most timeout_ms for each I/O operation.  Returns */
/* 1 if all the data was sent, 0 otherwise. */
static uint8_t opc_send(opc_sink sink, const uint8_t* data, ssize_t len, uint32_t timeout_ms) {
  opc_sink_info* info = &opc_sinks[sink];
  struct timeval timeout;
  ssize_t total_sent = 0;
  ssize_t sent;
  sig_t pipe_sig;

  if (sink < 0 || sink >= opc_next_sink) {
    fprintf(stderr, "OPC: Sink %d does not exist\n", sink);
    return 0;
  }
  if (!opc_connect(sink, timeout_ms)) {
    return 0;
  }
  timeout.tv_sec = timeout_ms/1000;
  timeout.tv_usec = timeout_ms % 1000;
  setsockopt(info->sockid, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
  while (total_sent < len) {
    pipe_sig = signal(SIGPIPE, SIG_IGN);
    sent = send(info->sockid, data + total_sent, len - total_sent, 0);
    signal(SIGPIPE, pipe_sig);
    if (sent <= 0) {
      perror("OPC: Error sending data");
      opc_close(sink);
      return 0;
    }
    total_sent += sent;
  }
  return 1;
}

uint8_t opc_put_pixels(opc_sink sink, uint8_t channel, uint16_t count, opc_pixel_t* pixels) {
  uint8_t header[4];
  ssize_t len;

  if (count > 0xffff / 3) {
    fprintf(stderr, "OPC: Maximum pixel count exceeded (%d > %d)\n",
            count, 0xffff / 3);
  }
  len = count * 3;

  header[0] = channel;
  header[1] = OPC_SET_PIXELS;
  header[2] = len >> 8;
  header[3] = len & 0xff;
  return opc_send(sink, header, 4, OPC_SEND_TIMEOUT_MS) &&
      opc_send(sink, (uint8_t*) pixels, len, OPC_SEND_TIMEOUT_MS);
}

int get_channel_size(int channel, const char *channel_file) {
  FILE *file = fopen(channel_file, "r");
  if (file == NULL) {
    perror("File could not be opened");
    exit(EXIT_FAILURE);
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
    exit(EXIT_FAILURE);
  }

  if (fclose(file)) {
    perror("File could not be closed");
    exit(EXIT_FAILURE);
  }

  perror("No pixel found at grid coordinate given");
  exit(EXIT_FAILURE);
}


// TODO: Read from a read only array mapping channels to their lengths
uint16_t get_channel_length(int channel) {
  return get_channel_size(channel, "../layout/channel_lengths.txt");
}

// Function to display a pixel list on the LEDs
uint8_t opc_put_pixel_list(opc_sink sink, opc_pixel_t **pixel_lists) {
  // Loop through each channel, writing their LEDs to the board
  for (uint8_t channel = 0; channel < NUM_STRIPS; channel++) {
    _status = opc_put_pixels(sink, (uint8_t) (channel + 1), get_channel_length(channel), pixel_lists[channel]);
  }

  return
}
