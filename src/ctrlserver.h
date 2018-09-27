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

#include "ctrlserver.h"

/**
 *  Get the specified interval of bits from an instruction, left padding with zeros
 *  Limits are inclusive.
 *
 *  @param inst: instruction to get bits from
 *  @param x: MSb of interval to return
 *  @param y: LSb of interval to return
 *  @return: word containing the specified bits, right aligned
 */
uint8_t bits_from_byte(char inst, uint8_t x, uint8_t y);

// TODO: figure out how this works and implement error checking?
// https://gist.github.com/barrysteyn/7308212
//Encodes a binary safe base 64 string
int base64_encode(const unsigned char* buffer, int length, char** b64text);

// Set up a socket to listen on
ctrl_server *start_server();

void clear_buff(char *buffer, size_t len);

// Accept connection from an incoming client, if there are any waiting
int try_accept_conn(ctrl_server *server);

// Read the latest input from the current connection into the buffer
// Return the size of the input read, or -1 if no input is waiting. Return 0 for client disconnect as usual
ssize_t get_latest_input(ctrl_server *server, char *buffer, size_t buffer_len);

// SHA1 hash and return the base64 of a key
void sha1_and_encode(char *key, char **b64hashed);

// Return true if the buffer contains a 'valid' HTTP upgrade to WS request, for our purposes
// TODO: Reject anything else that may or may not be a valid HTTP request?
bool is_valid_http_upgrade(char *request);

// Handle a valid HTTP upgrade to WebSocket request
// Append magic string to the client provided hash, then take the sha1 hash to send in response
int upgrade_to_ws(ctrl_server *server, char *request);

// Check we received a valid HTTP upgrade to WS request, then try to upgrade to a WS, returning 0 on success
int try_to_upgrade(ctrl_server *server, char *request);

// TODO: review errors and return codes
// Pre: there must be  a valid WS frame waiting
// Return -1 on error, or 0 if none or not enough data was waiting for a valid WS frame
char *read_ws_frame(ctrl_server *server);

// Ensure a client is disconnected, for whatever reason
void close_client(ctrl_server *server);

// Cleanup code
int close_server(ctrl_server *server);

void sleep_for(uint8_t s);


#endif //ARM11_22_CTRLSERVER_H
