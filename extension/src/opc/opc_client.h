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
#include "opc.h"

typedef struct {
  int sockid;
} opc_connection_t;

/* Wait at most 0.5 second for a connection or a write. */
#define OPC_SEND_TIMEOUT_MS 1000

/* Internal structure for a sink.  sock >= 0 iff the connection is open. */
typedef struct {
  struct sockaddr_in address;
  int sockid;
  char address_string[64];
} opc_sink_info;

static opc_sink_info opc_sinks[OPC_MAX_SINKS];
static opc_sink opc_next_sink = 0;

void opc_close(opc_sink sink);

#endif
