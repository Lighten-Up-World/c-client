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

#define CTRL_LISTEN_PORT "9090"
#define BASIC_LISTEN_PORT "9091"
#define HTTP_BUFFER 4095
#define BASIC_TCP_BUFFER 16
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
  int8_t shared_cmd;
  int interrupted;
  pthread_mutex_t mutex;
} server_args;

void *basic_server(void *);
void *server(void *);

#endif //ARM11_22_CTRLSERVER_H
