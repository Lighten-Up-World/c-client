#ifndef OPC_CLIENT_T
#define OPC_CLIENT_T

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include "../extension.h"

#include "../pixel.h"

#define OPC_DEFAULT_PORT 7890

/* OPC broadcast channel */
#define OPC_BROADCAST 0

/* OPC command codes */
#define OPC_SET_PIXELS 0

/* Maximum number of OPC sinks or sources allowed */
#define OPC_MAX_SINKS 64
#define OPC_MAX_SOURCES 64

/* Maximum number of pixels in one message */
#define OPC_MAX_PIXELS_PER_MESSAGE ((1 << 16) / 3)

typedef struct {
  int sockid;
} opc_connection_t;

/* Handle for an OPC sink created by opc_new_sink. */
typedef int8_t opc_sink;

/* Wait at most 0.5 second for a connection or a write. */
#define OPC_SEND_TIMEOUT_MS 1000

/* Internal structure for a sink.  sock >= 0 iff the connection is open. */
typedef struct {
  struct sockaddr_in address;
  int sockid;
  char address_string[64];
} opc_sink_info;


// OPC client functions ----------------------------------------------------

/* Creates a new OPC sink.  hostport should be in "host" or "host:port" form. */
/* No TCP connection is attempted yet; the connection will be automatically */
/* opened as necessary by opc_put_pixels, and reopened if it closes. */
opc_sink opc_new_sink(char* hostport);

/* Sends RGB data for 'count' pixels to channel 'channel'.  Makes one attempt */
/* to connect the sink if needed; if the connection could not be opened, the */
/* the data is not sent.  Returns 1 if the data was sent, 0 otherwise. */
uint8_t opc_put_pixels(opc_sink sink, uint8_t channel, uint16_t count, opc_pixel_t* pixels);

// Puts an entire map to the board, channel by channel
uint8_t opc_put_pixel_list(opc_sink sink, opc_pixel_t **channel_pixels);

void opc_close(opc_sink sink);

// OPC server functions ----------------------------------------------------

/* Handle for an OPC source created by opc_new_source. */
typedef int8_t opc_source;

/* Handler called by opc_receive when pixel data is received. */
typedef void opc_handler(uint8_t channel, uint16_t count, opc_pixel_t* pixels);

/* Creates a new OPC source by listening on the specified TCP port.  At most */
/* one incoming connection is accepted at a time; if the connection closes, */
/* the next call to opc_receive will begin listening for another connection. */
opc_source opc_new_source(uint16_t port);

// /* Handles the next I/O event for a given OPC source; if incoming data is */
// /* received that completes a pixel data packet, calls the handler with the */
// /* pixel data.  Returns 1 if there was any I/O, 0 if the timeout expired. */
// uint8_t opc_receive(opc_source source, opc_handler* handler, uint32_t timeout_ms);

/* Resets an OPC source to its initial state by closing the connection. */
void opc_reset_source(opc_source source);

#endif
