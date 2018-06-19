#ifndef APIMANAGER_H
#define APIMANAGER_H

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

int get_double_from_json(char *buf, char *name, char *object, double *val);
int get_string_from_json(char *buf, char *name, char *object, char *val);

int get_data_for_geolocation(int sockfd, geolocation_t *loc, char *host, char *path, char *key, char *buf, size_t buf_size);

#endif /* apimanagerh */
