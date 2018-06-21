#define _GNU_SOURCE

#include <string.h>
#include "../../src/utils/error.h"
#include "apimanager.h"
#include "utils/list.h"
#include "extension.h"
/**
 * SEND GET REQUEST
 * Sends a HTTP GET request and stores the result in a buffer
 *
 * @param sockfd - socket for the TCP/IP connection
 * @param request - struct representing the HTTP request
 * @param buf - where the retrieved json is stored
 * @param buf_size - size of storage for retrieved json
 * @return - 0 for success, -1 for failure.
 */
int send_get_request(int sockfd, http_request_t request, char *buf, size_t buf_size) {
  assert(buf != NULL);
  assert(request.method == GET);
  size_t req_size = 300;
  char req[req_size];
  size_t byte_count;

  // TODO: complete hack, fix if you want
  // TODO: fix 'write over read only memory' leak
  if (strcmp(request.host, "api.sunrise-sunset.org") == 0) {
    snprintf(req, req_size, "GET /%s HTTP/1.1\r\nHost:%s\r\nAccept:application/json\r\n\r\n", request.path, request.host);
  } else {
    snprintf(req, req_size, "GET /%s\r\nHTTP/1.1\r\nHost:%s\r\nAccept:application/json\r\n", request.path, request.host);
  }

  if (write(sockfd, req, req_size) < 0) {
    return -1;
  }
  byte_count = read(sockfd, buf, buf_size);
  printf("%s\n", buf);
  return byte_count;
}

/**
 * GET String FROM JSON
 * Takes in json and the attribute required (along with the object it contains) and writes string to val.
 *
 * @param buf - json to parse
 * @param name - name of the attribute that the double represents
 * @param object - the json object containing the attribute
 * @param val - string where the result is stored
 * @return - 0 for success, -1 for failure
 */
int get_string_from_json(char *buf, char *name, char *object, char *val) {
  assert(buf != NULL);
  assert(name != NULL);
  assert(val != NULL);
  assert(object != NULL);

  JSON_Value *root_value;
  root_value = json_parse_string(buf);

  if (json_value_get_type(root_value) != JSONObject) {
    printf("Not a json object \n");
    return -1;
  }
  JSON_Object *obj;
  obj = json_object_get_object(json_object(root_value), object);

  const char *str = json_object_get_string(obj, name);

  strncpy(val, str, strlen(str));

  json_value_free(root_value);

  return -(*val == 0);
}

/**
 * GET DOUBLE FROM JSON
 * Takes in json and the attribute required (along with the object it contains) and writes double to val.
 *
 * @param buf - json to parse
 * @param name - name of the attribute that the double represents
 * @param object - the json object containing the attribute
 * @param val - pointer to the double where the result is stored
 * @return - 0 for success, -1 for failure
 */
int get_double_from_json(char *buf, char *name, char *object, double *val) {
  assert(buf != NULL);
  assert(name != NULL);
  assert(val != NULL);
  assert(object != NULL);

  JSON_Value *root_value;
  root_value = json_parse_string(buf);
  if (json_value_get_type(root_value) != JSONObject) {
    printf("Not a json object \n");
    return -1;
  }
  JSON_Object *obj;
  obj = json_object_get_object(json_object(root_value), object);
  *val = json_object_get_number(obj, name);
  json_value_free(root_value);
  return 0;
}

int get_data_for_geolocation(int sockfd, geolocation_t *loc, char *host, char *path, char *key, char *buf, size_t buf_size){
  assert(loc != NULL);
  assert(host != NULL);
  assert(path != NULL);
  assert(buf != NULL);
  assert(key != NULL);

  http_request_t request;
  request.host = host;
  request.method = GET;
  asprintf(&(request.path), path, loc->latitude, loc->longitude, key);
  if (send_get_request(sockfd, request, buf, buf_size) < 0) {
    free(request.path);
    return -1;
  }
  free(request.path);
  return 0;
}

/**
 * SOCKET CONNECT
 * Connects a socket using the hostname and port.
 *
 * @param host : hostname to connect
 * @param port : port to connect with
 * @return : socket number
 */
int socket_connect(const char *host, in_port_t port) {
  assert(host != NULL);
  struct hostent *hp;
  struct sockaddr_in addr;
  int on = 1, sock;

  if ((hp = gethostbyname(host)) == NULL) {
    herror("gethostbyname");
    exit(1);
  }

  memcpy(&addr.sin_addr, hp->h_addr, hp->h_length);
  addr.sin_port = htons(port);
  addr.sin_family = AF_INET;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *) &on, sizeof(int));

  if (sock == -1) {
    perror("setsockopt");
    exit(1);
  }

  if (connect(sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1) {
    perror("connect");
    exit(1);
  }
  return sock;
}

/**
 * SOCKET CLOSE
 * Closes a socket.
 *
 * @param sockfd - socket to close
 * @return 0 for success, -1 for failure.
 */
int socket_close(int sockfd) {
  if (close(sockfd) == -1) {
    perror("socket_close");
    return -1;
  }
  return 0;
}
