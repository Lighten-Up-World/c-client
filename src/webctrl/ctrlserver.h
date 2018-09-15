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
#include <openssl/sha.h>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#define LISTEN_PORT "9090"
#define TCP_BUFFER 4095
#define BACKLOG 0

// 20 bytes // 160 bits
//#define SHA1_CHAR_LEN (160 / (sizeof(char) * 8))
#define SHA1_CHAR_LEN 20

// 29 bytes // based on calculation using SHA1_CHAR_LEN
//#define SHA1_ENCODED_LEN ((size_t) ceil((double)(4 * SHA1_CHAR_LEN) / 3))
#define SHA1_ENCODED_LEN 28

#define WEBSOCKET_KEY_HEADER "Sec-WebSocket-Key: "
#define SEC_WEBSOCKET_MAGIC "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define MASKING_KEY_LEN 4

#define RESPONSE_START "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: "
#define RESPONSE_END "\r\n\r\n"

typedef struct {
  int socket_fd;
  int client_fd;
  char *buffer;
} ctrl_server;

#endif //ARM11_22_CTRLSERVER_H
