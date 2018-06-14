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
#include "api.h"
#include "jsmn/jsmn.h"

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

int get_value_for_geolocation(geolocation_t *loc, char *host, char *path ,char *attr);

int get_int_from_json(char *buf, char *name, int *val);

int send_get_request(int sock, http_request_t request, char *buf, size_t buf_size);

int socket_connect(const char *host, in_port_t port);
int socket_close(int sockfd);

#endif /* apimanager_h */