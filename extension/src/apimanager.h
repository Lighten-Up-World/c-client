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

//Forward Defs
typedef struct apimanager api_manager_t;
typedef struct api api_t;

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

typedef int (*get_pixel_func) (api_manager_t *self, int pos, opc_pixel_t *pixel, void *obj);
typedef int (*api_manager_func)(api_manager_t *self);

struct api {
  char *name;
  get_pixel_func get_pixel;
};

struct apimanager{
  api_t *api;
  list_t *pixel_info;
  api_manager_func construct;
  api_manager_func destruct;
  void *obj;
};

api_manager_t *api_manager_new(void);
int api_manager_delete(api_manager_t *self);
int api_manager_init(api_manager_t *self, api_t *api, list_t *pixel_info);

int socket_connect(const char *host, in_port_t port);
int socket_close(int sockfd);

int get_value_for_geolocation(int sockfd, geolocation_t *loc, char *host,
                              char *path, char *key, char *attr, char *object, double *val);
#endif /* apimanagerh */
