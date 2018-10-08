#include "ctrlserver.h"

// TODO: figure out how this works and implement error checking?
// https://gist.github.com/barrysteyn/7308212
//Encodes a binary safe base 64 string
int base64_encode(const unsigned char* buffer, int length, char** b64text) {
  BIO *bio, *b64;
  BUF_MEM *bufferPtr;

  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new(BIO_s_mem());
  bio = BIO_push(b64, bio);

  //Ignore newlines - write everything in one line
  BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
  BIO_write(bio, buffer, length);
  BIO_flush(bio);
  BIO_get_mem_ptr(bio, &bufferPtr);
  BIO_set_close(bio, BIO_NOCLOSE);
  BIO_free_all(bio);

  *b64text = bufferPtr->data;

  return 0;
}

// Set up a socket to listen on
ctrl_server *start_server() {
  struct addrinfo hints;
  struct addrinfo *sock_addr;
  int sockfd;

  // Create socket address
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;        // IPv4
  hints.ai_socktype = SOCK_STREAM;  // TCP
  hints.ai_flags = AI_PASSIVE;      // Auto fill computers IP
  getaddrinfo(NULL, LISTEN_PORT, &hints, &sock_addr);

  // Create socket
  if ((sockfd = socket(sock_addr->ai_family, sock_addr->ai_socktype, sock_addr->ai_protocol)) < 0) {
    perror("Socket");
    exit(errno);
  }

  // Set the socket to non-blocking
  fcntl(sockfd, F_SETFL, O_NONBLOCK);

  // Assign port to socket
  if (bind(sockfd, sock_addr->ai_addr, sock_addr->ai_addrlen)) {
    perror("Socket--bind");
    exit(errno);
  }

  // Make it a "listening socket"
  if (listen(sockfd, BACKLOG)) {
    perror("Socket--listen");
    exit(errno);
  }

  ctrl_server *server = calloc(1, sizeof(ctrl_server));
  if (server == NULL) {
    perror("Server struct");
    exit(errno);
  }
  server->socket_fd = sockfd;
  server->client_fd = 0;

  return server;
}

void clear_buff(char *buffer, size_t len) {
  for (int i = 0; i < len; i++) {
    buffer[i] = '\0';
  }
}

// Accept connection from an incoming client, if there are any waiting
int try_accept_conn(ctrl_server *server) {
  struct sockaddr_in client_addr;
  size_t size = sizeof(client_addr);
  int client_sock = accept(server->socket_fd, (struct sockaddr *) &client_addr, (socklen_t*) &size);

  if (client_sock < 0) {
    // No client is waiting
    if (errno == EAGAIN || errno ==  EWOULDBLOCK) {
      return 0;
      // Other error
    } else {
      perror("accept");
      exit(errno);
    }
  }

  // Client connected
  printf("Client connected from host %s, port %hu\n",
         inet_ntoa(client_addr.sin_addr),
         ntohs (client_addr.sin_port));

  return client_sock;
}

// Read the latest input from the current connection into the buffer
// Return the size of the input read, or -1 if no input is waiting. Return 0 for client disconnect as usual
ssize_t get_latest_input(ctrl_server *server, char *buffer, size_t buffer_len) {
  // Clear buffer and get all waiting data from client in queue (up to buffer size)
  clear_buff(buffer, buffer_len);
  ssize_t read_size = recv(server->client_fd, buffer, buffer_len, MSG_DONTWAIT);

  // No input is waiting
  if (read_size == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return -1;
    } else {
      perror("recv");
      exit(EXIT_FAILURE);
    }
  }

  // Client disconnected
  if (read_size == 0) {
    return 0;
  }

  // Other error
  if (read_size < 0) {
    perror("receive");
    exit(errno);
  }

  return read_size;
}

// SHA1 hash and return the base64 of a key
void sha1_and_encode(char *key, char **b64hashed) {
  printf("key: %s\n", key);
  
  // Calculate the SHA1 hash
  unsigned char *hash = calloc(SHA1_CHAR_LEN, sizeof(char));
  if (hash == NULL) {
    perror("Hash alloc");
    exit(errno);
  }
  hash = SHA1((const unsigned char *)key, strlen((const char *) key), hash);

  puts("hash:");
  for (int i=0; i < 20; i++) {
    printf("%02x ", hash[i]);
  }
  puts("");

  // Encode the hash in base64 before placing in response
  base64_encode(hash, SHA1_CHAR_LEN, b64hashed);
  (*b64hashed)[SHA1_ENCODED_LEN] = '\0';
 
  puts("encoded:");
  for (int i=0; i < 28; i++) {
    printf("%c", (*b64hashed)[i]);
  }
  puts("");

  free(hash);
}

// Return true if the buffer contains a 'valid' HTTP upgrade to WS request, for our purposes
// TODO: Reject anything else that may or may not be a valid HTTP request?
bool is_valid_http_upgrade(char *request) {
  if (strstr(request, REQUEST_METHOD) != request) return false;
  if (strstr(request, REQUEST_UPGRADE) == NULL) return false;
  if (strstr(request, UPGRADE_TO_WS) == NULL) return false;
  if (strstr(request, WS_KEY_HEADER) == NULL) return false;
  return true;
}

// Handle a valid HTTP upgrade to WebSocket request
// Append magic string to the client provided hash, then take the sha1 hash to send in response
int upgrade_to_ws(ctrl_server *server, char *request) {
  printf("request: %s", request);

  char *key_header_start = strstr(request, WS_KEY_HEADER);
  char *key_start = strchr(key_header_start, ' ') + 1;
  if (key_start == NULL || key_start != key_header_start + strlen(WS_KEY_HEADER)) {
    perror("Sec-WebSocket-Key header is malformed (key start)");
    exit(errno);
  }

  char *key_end = strchr(key_start, '\r');
  if (key_end == NULL) {
    perror("Sec-WebSocket-Key header is malformed (key end)");
    exit(errno);
  }

  size_t key_len = key_end - key_start;
  char *key = calloc(key_len + strlen(WS_KEY_MAGIC) + 1, sizeof(char));
  if (key == NULL) {
    perror("Key alloc");
    exit(errno);
  }
  memcpy(key, key_start, key_len);
  memcpy(key + key_len, WS_KEY_MAGIC, strlen(WS_KEY_MAGIC));

  printf("key: %s\n", key);

  char *b64hashed = calloc(SHA1_ENCODED_LEN + 1, sizeof(char));
  sha1_and_encode(key, &b64hashed);
  free(key);

  clear_buff(request, HTTP_BUFFER);
  strcat(request, RESPONSE_START);
  strncat(request, b64hashed, SHA1_ENCODED_LEN);
  strcat(request, RESPONSE_END);

  printf("response: %s", request);

  free(b64hashed);

  // 0: no bytes written, -1: error and errno is set
  return (int) (write(server->client_fd, request, strlen(request)) <= 0);
}

// Check we received a valid HTTP upgrade to WS request, then try to upgrade to a WS, returning 0 on success
int try_to_upgrade(ctrl_server *server, char *request) {
  return (is_valid_http_upgrade(request)) ? upgrade_to_ws(server, request) : 1;
}

// Ensure a client is disconnected, for whatever reason
void close_client(ctrl_server *server) {
  if (close(server->client_fd)) {
    perror("Client close");
    exit(EXIT_FAILURE);
  }
  server->client_fd = 0;
}

// TODO: review errors and return codes
// Pre: there must be  a valid WS frame waiting
// Return -1 on error, or 0 if none or not enough data was waiting for a valid WS frame
int read_ws_frame(ctrl_server *server) {
  // Data is received in network order
  ssize_t read;
  char byte;

  // FIN, RSV 123, OPCODE
  read = get_latest_input(server, &byte, 1);
  if (read <= 0) return -1;
  int fin = byte & 0x80 ? 1 : 0;
  int rsv_zero = byte & 0x70; // For actual value, shift right
  int opcode = byte & 0xf;
  if (rsv_zero != 0) {
    perror("rsv was not 1");
    exit(EXIT_FAILURE);
  }
  printf("\tfin: %d\n", fin);
  printf("\trsv: %d\n", rsv_zero);
  printf("\topcode: 0x%x\n", opcode);

  // MASK, PAYLOAD LENGTH
  read = get_latest_input(server, &byte, 1);
  if (read <= 0) return -1;
  int mask = byte & 0x80 ? 1 : 0;
  int payload_len = byte & 0x7f;
  if (mask != 1) {
    perror("mask bit was not 1");
    exit(EXIT_FAILURE);
  }
  if (payload_len > 125) {
    perror("payload length unexpectedly long");
    exit(EXIT_FAILURE);
  }
  printf("\tmask: %d\n", mask);
  printf("\tpayload length: %d\n", payload_len);

  // MASKING KEY
  char *masking_key = calloc(WS_MASKING_KEY_LEN, sizeof(char));
  read = get_latest_input(server, masking_key, WS_MASKING_KEY_LEN);
  if (read <= 0) return -1;

  // PAYLOAD
  if (payload_len <= 0) return -1;
  char *payload = calloc((size_t) payload_len, sizeof(char));
  read = get_latest_input(server, payload, (size_t) payload_len);
  if (read <= 0) return -1;

  // Apply mask to payload
  char *decoded = calloc((size_t) payload_len + 1, sizeof(char));
  for (int i = 0; i < payload_len; i++) {
    decoded[i] = payload[i] ^ masking_key[i%4];
  }
  decoded[payload_len] = '\0';
  printf("\tpayload: %s\n", decoded);

  if (opcode > 0) {
    switch (opcode) {
      case 0x1:
        break;
      case 0x8:
        puts("Terminate");
        return 0;
      case 0x9:
        puts("Ping");
        return 0;
      default:
        puts("Unexpected");
        return 0;
    }
  } else if (opcode == 0) {
    puts("Client disconnected");
    return 0;
  }

  long conv = strtol(decoded, NULL, 0);
  if (conv == LONG_MIN || conv == LONG_MAX || conv == 0) {
    exit(errno);
  }

  return (int) (conv - 1);
}

// Cleanup code
void close_server(ctrl_server *server) {
  if (server == NULL) {
    return;
  }
  if (server->client_fd) {
    if (close(server->client_fd)) {
      perror("Closing client fd failed");
      exit(errno);
    }
  }
  if (server->socket_fd) {
    if (close(server->socket_fd)) {
      perror("Closing socket fd failed");
      exit(errno);
    }
  }
  free(server);
}

// Sleep for s seconds
void sleep_for(uint8_t s) {
  struct timespec sleep;
  sleep.tv_nsec = 0;
  sleep.tv_sec = s;
  nanosleep(&sleep, NULL);
}

void *server(void *as) {
  // Set up the server
  server_args *args = (server_args *) as;
  ctrl_server *server = start_server();
  int read;

  // Run forever
  while (!args->interrupted) {
    // If client is connected
    if (server->client_fd) {
      read = read_ws_frame(server);
      if (read > 0) {
        args->shared_cmd = (uint8_t) read;
      } else if (read == 0) {
        puts("Client disconnected");
        close_client(server);
      }
    } else {
      // Check if any client is waiting to connect
      server->client_fd = try_accept_conn(server);

      // If a connection was accepted, block until it's upgraded it to a WebSocket
      if (server->client_fd) {
        ssize_t read_size;
        int tries = 0;
        char *http_buffer = calloc(HTTP_BUFFER + 1, sizeof(char));
        do {
          read_size = get_latest_input(server, http_buffer, HTTP_BUFFER);
          tries++;
        } while (read_size < 0 && tries < 5);

        if (read_size > 0) {
          puts("Upgrading to WebSocket...");
          if (try_to_upgrade(server, http_buffer)) {
            close_client(server);
          }
        } else {
          close_client(server);
        }
        free(http_buffer);
      }
    }
    sleep_for(1);
  }
  close_server(server);
  return NULL;
}
