//
//  apimanager.h
//  LightenUpWorld
//
//  Created by User on 13/06/2018.
//  Copyright © 2018 User. All rights reserved.
//

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

//Forward Defs
typedef struct api_manager api_manager_t;
typedef struct geolocation geolocation_t;

#include "projection.h"

typedef int (*get_pixel_func) (api_manager_t *self, pixel_t *pixel, void *obj);
typedef int (*api_manager_construct)(api_manager_t *self);
typedef int (*api_manager_destruct)(api_manager_t *self);

struct api_manager {
    get_pixel_func get_pixel;
    api_manager_construct construct;
    api_manager_destruct destruct;
    void *obj;
};


//GENERAL

struct geolocation{
    double latitude;
    double longitude;
};

typedef enum http_request_method{
    GET,
    PUT,
    POST
} http_request_method_t;

typedef struct http_request{
    http_request_method_t method;
    char *host;
    char *path;
}http_request_t;

int tcp_construct(api_manager_t *api_manager, char *host);
int tcp_destruct(api_manager_t *api_manager);

api_manager_t *api_manager_new(void);
int api_manager_delete(api_manager_t *self);

int get_value_for_geolocation(int sockfd, geolocation_t *loc, char *host, char *path ,char *attr, double *val);

#endif /* apimanager_h */
