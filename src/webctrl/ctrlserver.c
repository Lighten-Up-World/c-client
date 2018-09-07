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

  // Set up server struct //TODO: check if we need this +1
  char *buffer = calloc(TCP_BUFFER + 1, sizeof(char));
  ctrl_server *server = calloc(1, sizeof(ctrl_server));
  if (server == NULL) {
    perror("Server struct");
    exit(errno);
  }
  server->socket_fd = sockfd;
  server->client_fd = 0;
  server->buffer = buffer;

  return server;
}

void clear_buffer(ctrl_server *server) {
  for (int i = 0; i < TCP_BUFFER; i++) {
    server->buffer[i] = '\0';
  }
}

// Accept connection from an incoming client, if there are any waiting
int try_accept_conn(ctrl_server *server) {
  struct sockaddr_in client_addr;
  size_t size = sizeof(client_addr);
  int client_sock = accept(server->socket_fd, (struct sockaddr *) &client_addr, (socklen_t*) &size);

  // No client is waiting
  if (errno == EAGAIN || errno ==  EWOULDBLOCK) {
    return 0;
  }

  // Other error
  if (client_sock < 0) {
    perror("accept");
    exit(errno);
  }

  // Client connected
  printf("Client connected from host %s, port %hu\n",
         inet_ntoa(client_addr.sin_addr),
         ntohs (client_addr.sin_port));

  return client_sock;
}

// Read the latest input from the current connection into the buffer
// Return the size of the input read, or -1 if no input is waiting. Return 0 for client disconnect as usual
ssize_t get_latest_input(ctrl_server *server) {
  // Clear buffer and get all waiting data from client in queue (up to buffer size)
  clear_buffer(server);
  ssize_t read_size = recv(server->client_fd, server->buffer, TCP_BUFFER, MSG_DONTWAIT);

  // No input is waiting
  if (errno == EAGAIN || errno == EWOULDBLOCK) {
    return -1;
  }

  // Client disconnected
  if (read_size == 0) {
    puts("Client disconnected");
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
// TODO: check b64 max size given sha1 returns 20 bytes, and b64 returns 28 or less chars
void sha1_and_encode(char *key, char **b64hashed) {
  // Calculate the SHA1 hash
  unsigned char *hash = calloc(SHA1_CHAR_LEN, sizeof(char));
  if (hash == NULL) {
    perror("Hash alloc");
    exit(errno);
  }
  hash = SHA1((const unsigned char *)key, strlen((const char *) key), hash);

  // Encode the hash in base64 before placing in response
  base64_encode(hash, SHA1_CHAR_LEN, b64hashed);
  (*b64hashed)[SHA1_ENCODED_LEN] = '\0';

  free(hash);
}

// Return true if the buffer contains a 'valid' HTTP 101, for our purposes
bool is_valid_http_upgrade(ctrl_server *server) {
  // TODO: Check what a valid format looks like online

  // Basics: (check http version)
  // GET / HTTP/1.1
  // Connection: Upgrade
  // Upgrade: websocket
  // Sec-WebSocket-Key: ...

  // Reject anything else that may or may not be a valid HTTP request

  return true;
}

// Handle a valid HTTP upgrade to WebSocket request
int upgrade_to_ws(ctrl_server *server) {
  // Append magic string to the client provided hash, then take the sha1 hash to send in response
  char *key_header_start = strstr(server->buffer, WEBSOCKET_KEY_HEADER);
  if (key_header_start == NULL) {
    perror("Sec-WebSocket-Key not present");
    exit(errno);
  }

  char *key_start = strchr(key_header_start, ' ') + 1;
  if (key_start == NULL || key_start != key_header_start + strlen(WEBSOCKET_KEY_HEADER)) {
    perror("Sec-WebSocket-Key header is malformed (key start)");
    exit(errno);
  }

  char *key_end = strchr(key_start, '\r');
  if (key_end == NULL) {
    perror("Sec-WebSocket-Key header is malformed (key end)");
    exit(errno);
  }

  size_t key_len = key_end - key_start;
  char *key = calloc(key_len + strlen(SEC_WEBSOCKET_MAGIC), sizeof(char));
  if (key == NULL) {
    perror("Key alloc");
    exit(errno);
  }
  memcpy(key, key_start, key_len);
  memcpy(key + key_len, SEC_WEBSOCKET_MAGIC, strlen(SEC_WEBSOCKET_MAGIC));

  char *b64hashed = calloc(SHA1_ENCODED_LEN + 1, sizeof(char));
  sha1_and_encode(key, &b64hashed);
  printf("key: %s\n", key);
  printf("b64: %s\n", b64hashed);
  free(key);

  clear_buffer(server);
  strcat(server->buffer, RESPONSE_START);
  strncat(server->buffer, b64hashed, SHA1_ENCODED_LEN);
  strcat(server->buffer, RESPONSE_END);
  free(b64hashed);

  // TODO: handle errors here properly
  // if 0, no bytes written, -1 is error and errno is set
  return (int) (write(server->client_fd, server->buffer, strlen(server->buffer)) == 0);
}

// Upgrade if possible, returning 0 on success
int try_to_upgrade(ctrl_server *server) {
  // Check we received a valid HTTP 101 upgrade to WS
  return (is_valid_http_upgrade(server)) ? upgrade_to_ws(server) : 1;
}

// Handle input from client, after input is read into buffer
int handle_input(ctrl_server *server) {
  for (int i = 0; i < 32; i++) {
    printf("%d: %08x ", i, server->buffer[i]);
  }
  //printf("Buffer: \n%s", server->buffer); // TODO: fix seg fault here

  // Get a pointer to the last command in the buffer
  /*char *last_cmd = extract_last_cmd(server);
  printf("Processing last cmd: %s", last_cmd);

  if (strncmp(last_cmd, "echo\n", 4) == 0) {
    return (int) write(server->client_fd, server->buffer, strlen(server->buffer));
  }

  return (int) write(server->client_fd, server->buffer, strlen(server->buffer));*/
  return 0;
}

// Cleanup code
int close_server(ctrl_server *server) {
  if (server == NULL) {
    return 0;
  }
  if (server->buffer) {
    free(server->buffer);
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
  return 0;
}

// Sleep for s seconds
void sleep_for(uint8_t s) {
  struct timespec sleep;
  sleep.tv_nsec = 0;
  sleep.tv_sec = s;
  nanosleep(&sleep, NULL);
}

// Set interrupted flag
int interrupted = 0;
void handle_user_exit(int _) {
  interrupted = 1;
}

// Testing only
// TODO: add button on map to reset connection (set client = 0), go back to listening
// TODO: check why message is only displayed after ctrl-c
int main() {
  // Set up Ctrl+C handle
  signal(SIGINT, handle_user_exit);

  // Create socket, setup server
  ctrl_server *server = start_server();

  // Simulates main loop of effect runner
  while (!interrupted) {

    // *** Effect runner code here ***

    // If client is connected
    if (server->client_fd) {
      // Handle input from client if any was received
      ssize_t read_size = get_latest_input(server);
      if (read_size > 0) {
        puts("Handling...");
        handle_input(server);
      } else if (read_size == 0) { //TODO: handle websocket disconnect message
        puts("Disconnected");
        server->client_fd = 0;
      }
    } else {
      // Check if any client is waiting to connect
      server->client_fd = try_accept_conn(server);

      // If a connection was accepted, wait until it's upgraded it to a WebSocket
      if (server->client_fd) {
        // Block until we have an input from the connection.
        ssize_t read_size;
        int tries = 0;
        do {
          sleep_for(1); //TODO: make this shorter
          read_size = get_latest_input(server);
          tries++;
        } while (read_size < 0 && tries < 5);

        if (read_size > 0) {
          puts("Upgrading to websocket...");
          if (try_to_upgrade(server)) {
            // If upgrade failed, disconnect the client
            close(server->client_fd);
            server->client_fd = 0;
          }
        } else {
          server->client_fd = 0;
        }
      }

    }
    sleep_for(1);
  }

  close_server(server);
}







