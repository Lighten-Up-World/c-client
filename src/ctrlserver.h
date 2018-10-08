#ifndef ARM11_22_CTRLSERVER_H
#define ARM11_22_CTRLSERVER_H

#include <stdio.h>
#include <sys/socket.h>
#include <resolv.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>
#include <math.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <assert.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <pthread.h>

#define LISTEN_PORT "9090"
#define HTTP_BUFFER 4095
#define BACKLOG 0

#define SHA1_CHAR_LEN 20    // 20 bytes // 160 bits
#define SHA1_ENCODED_LEN 28 // 28 characters // calculated using SHA1_CHAR_LEN

#define REQUEST_METHOD "GET / HTTP/"
#define REQUEST_UPGRADE "Connection: Upgrade\r\n"
#define UPGRADE_TO_WS "Upgrade: websocket\r\n"
#define WS_KEY_HEADER "Sec-WebSocket-Key: "
#define WS_KEY_MAGIC "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define WS_MASKING_KEY_LEN 4

#define RESPONSE_START "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: "
#define RESPONSE_END "\r\n\r\n"

typedef struct {
  int socket_fd;
  int client_fd;
} ctrl_server;

typedef struct {
  uint8_t shared_cmd;
  int interrupted;
  pthread_mutex_t mutex;
} server_args;

//int base64_encode(const unsigned char* buffer, int length, char** b64text);
//ctrl_server *start_server();
//void clear_buff(char *buffer, size_t len);
//int try_accept_conn(ctrl_server *server);
//ssize_t get_latest_input(ctrl_server *server, char *buffer, size_t buffer_len);
//void sha1_and_encode(char *key, char **b64hashed);
//bool is_valid_http_upgrade(char *request);
//int upgrade_to_ws(ctrl_server *server, char *request);
//int try_to_upgrade(ctrl_server *server, char *request);
//char *read_ws_frame(ctrl_server *server);
//void close_client(ctrl_server *server);
//int close_server(ctrl_server *server);
//void sleep_for(uint8_t s);
void *server(void *);

#endif //ARM11_22_CTRLSERVER_H
