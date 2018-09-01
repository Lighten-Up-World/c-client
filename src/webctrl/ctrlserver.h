#ifndef ARM11_22_CTRLSERVER_H
#define ARM11_22_CTRLSERVER_H

#include <stdio.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
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
#include <openssl/sha.h>

#define LISTEN_PORT "9090"
#define BUFFER 1024
#define BACKLOG 0
#define CMD_TERMINATOR '\n'

#define WEBSOCKET_KEY_HEADER "Sec-WebSocket-Key: "
#define SEC_WEBSOCKET_MAGIC "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

#endif //ARM11_22_CTRLSERVER_H
