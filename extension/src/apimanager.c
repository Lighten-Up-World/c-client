//
//  apimanager.c
//  LightenUpWorld
//
//  Created by User on 13/06/2018.
//  Copyright © 2018 User. All rights reserved.
//
#define _GNU_SOURCE
#include "apimanager.h"

int get_value_for_geolocation(geolocation_t *loc, char *host, char *path ,char *attr){
  assert(loc != NULL);
  assert(host != NULL);
  assert(attr != NULL);
  assert(path != NULL);

  int sockfd = socket_connect(host, 80);
  http_request_t request;
  request.host = host;
  request.method = GET;

  /*
  char *lon = calloc(4, sizeof(char));
  snprintf(lon, 4, "%d",loc->longitude);
  char *lat = calloc(4, sizeof(char));
  snprintf(lat, 4, "%d", loc->latitude);

  strncpy(path + LATITUDE_START + 3 - strlen(lat), lat, strlen(lat));
  strncpy(path + LONGITUDE_START + 3 - strlen(lon), lon, strlen(lon));

  free(lat);
  free(lon);
   */

  asprintf(&(request.path),path, loc->latitude, loc->longitude);

  char buff[500];
  send_get_request(sockfd, request, buff, sizeof(buff));

  return get_double_from_json(buff, attr, &(loc->value));
}

int send_get_request(int sockfd, http_request_t request, char *buf, size_t buf_size){
  assert(buf != NULL);
  assert(request.method == GET);
  char req[300];
  size_t byte_count;
  snprintf(req, sizeof(req), "GET /%s\r\nHTTP/1.1\r\nHost:%s\nConnection:keep-alive\r\n",request.path, request.host);
  //printf("%s \n", req);
  if (send(sockfd,req,sizeof(req),0) < 0) {
    return -1;
  }
  byte_count = recv(sockfd,buf,buf_size,0);
  return byte_count;
}

int get_double_from_json(char *buf, char *name, double *val){
  assert(buf != NULL);
  assert(name != NULL);
  assert(val != NULL);
  JSON_Value *root_value;
  root_value = json_parse_string(buf);

  if (json_value_get_type(root_value) != JSONObject) {
    printf("Not a json object \n");
    return -1;
  }
  JSON_Object *obj;
  obj = json_object_get_object(json_object(root_value), "main");
  *val = json_object_get_number (obj, name);
  return -(*val == 0);
}

int socket_connect(const char *host, in_port_t port){
  assert(host != NULL);
  struct hostent *hp;
  struct sockaddr_in addr;
  int on = 1, sock;

  if((hp = gethostbyname(host)) == NULL){
    herror("gethostbyname");
    exit(1);
  }

  bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
  addr.sin_port = htons(port);
  addr.sin_family = AF_INET;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

  if(sock == -1){
    perror("setsockopt");
    exit(1);
  }

  if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
    perror("connect");
    exit(1);
  }
  return sock;
}

int socket_close(int sockfd){
  if (shutdown(sockfd, 2) == -1) {
    perror("socket_close");
    return -1;
  }
  return 0;
}
