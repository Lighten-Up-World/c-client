#define _GNU_SOURCE

#include "../../src/utils/error.h"
#include "apimanager.h"

api_manager_t *api_manager_new(void){
  api_manager_t *api_manager = NULL;
  api_manager = malloc(sizeof(api_manager_t));
  if(api_manager == NULL){
    perror("api_manager_new failed");
    exit(EXIT_FAILURE);
  }

  return api_manager;
}

int api_manager_init(api_manager_t *self, api_t *api, list_t *pixel_info){
  if(self == NULL){
    return EC_INVALID_PARAM;
  }
  self->api = api;
  self->pixel_info = pixel_info;
  return EC_OK;
}

int api_manager_delete(api_manager_t *self){
  free(self);
  return EC_OK;
}

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
  char req[300];
  size_t byte_count;
  snprintf(req, sizeof(req), "GET /%s\nHTTP/1.1\r\nHost:%s\nConnection:keep-alive\r\n", request.path, request.host);
  if (send(sockfd, req, sizeof(req), 0) < 0) {
    return -1;
  }
  byte_count = recv(sockfd, buf, buf_size, 0);
  return byte_count;
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
  return -(*val == 0);
}

/**
 * GET VALUE FOR GEO-LOCATION
 * Passes socket and relevant parsing information to retrieve a particular value at a geolocation using a HTTP request.
 *
 * @param sockfd - socket for the TCP/IP connection
 * @param loc - pointer to the geolocation
 * @param host - hostname
 * @param path - url path format string
 * @param attr - name of attribute to be obtained
 * @param object - name of json object in which the attribute will be contained
 * @param val - pointer to the double where the value is stored.
 * @return
 */

int get_value_for_geolocation(int sockfd, geolocation_t *loc, char *host, char *path, char *key, char *attr, char *object,
                              double *val) {
  assert(loc != NULL);
  assert(host != NULL);
  assert(object != NULL);
  assert(attr != NULL);
  assert(path != NULL);

  http_request_t request;
  request.host = host;
  request.method = GET;

  asprintf(&(request.path), path, loc->latitude, loc->longitude, key);

  char buff[600];
  if (send_get_request(sockfd, request, buff, sizeof(buff)) < 0) {
    return -1;
  }

  return get_double_from_json(buff, attr, object, val);
}

/***
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

/***
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
