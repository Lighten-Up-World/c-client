#ifndef apimanager_h
#define apimanager_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include "pixel.h"
#include "parson/parson.h"
#include "utils/list.h"

#include "projection.h"

typedef enum http_request_method{
  GET,
  PUT,
  POST
} http_request_method_t;

typedef struct http_request{
  http_request_method_t method;
  char *host;
  char *path;
} http_request_t;

int socket_connect(const char *host, in_port_t port);
int socket_close(int sockfd);

int get_value_for_geolocation(int sockfd, geolocation_t *loc, char *host,
                              char *path, char *key, char *attr, char *object, double *val);
#endif /* apimanagerh */
