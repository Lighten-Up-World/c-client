#include "ctrlserver.h"

// TODO: figure out how this works and implement error checking?
// https://gist.github.com/barrysteyn/7308212

// TODO: fix extra character at end of return buffer
//Encodes a binary safe base 64 string
int Base64Encode(const unsigned char* buffer, size_t length, char** b64text) {
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
  char *buffer = calloc(BUFFER + 1, sizeof(char));
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
  for (int i = 0; i < BUFFER; i++) {
    server->buffer[i] = '\0';
  }

  ssize_t read_size = recv(server->client_fd, server->buffer, BUFFER, MSG_DONTWAIT);

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

// Return true if the buffer contains a 'valid' HTTP 101, for our purposes

int is_valid_http_upgrade(ctrl_server *server) {
  // TODO: Check what a valid format looks like online

  // Basics: (check http version)
  // GET / HTTP/1.1
  // Connection: Upgrade
  // Upgrade: websocket
  // Sec-WebSocket-Key: ...

  // Reject anything else that may or may not be a valid HTTP request

  return 0;
}

// Handle a valid HTTP upgrade to WebSocket request
// TODO: check how to identify this and code it
int upgrade_to_ws(ctrl_server *server) {
  // Extract client hash
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

  // Append magic string to the client provided hash, then take the sha1 hash to send in response
  size_t key_len = key_end - key_start;
  char *key = calloc(key_len + strlen(SEC_WEBSOCKET_MAGIC), sizeof(char));
  if (key == NULL) {
    perror("Key alloc");
    exit(errno);
  }
  memcpy(key, key_start, key_len);
  memcpy(key + key_len, SEC_WEBSOCKET_MAGIC, strlen(SEC_WEBSOCKET_MAGIC));

  unsigned char *hash = calloc(21, sizeof(char));
  if (hash == NULL) {
    perror("Hash alloc");
    exit(errno);
  }
  SHA1((const unsigned char *)key, strlen((const char *) key), hash);

  // make sure null byte is set, TODO: use strncpy_s above instead??
  hash[20] = '\0';

  // base64 encode the output
  char *base64hash;
  Base64Encode(hash, strlen((const char *) hash), &base64hash);

  char *response_start = "HTTP/1.1 101 Switching Protocols\r\n"
                         "Upgrade: websocket\r\n"
                         "Connection: Upgrade\r\n"
                         "Sec-WebSocket-Accept: ";
  char *response_end = "\r\n\r\n";
  char *response = calloc(sizeof(char), strlen(response_start) + strlen(response_end) + strlen(base64hash) + 1);
  if (response == NULL) {
    perror("Response alloc");
    exit(errno);
  }
  strcat(response, response_start);
  strcat(response, base64hash);
  strcat(response, response_end);
  printf("%s", response);

  // TODO: fix error with extra character at end of base64 hash
  // TODO: use buffer instead of response, set buffer to an appropriate size

  free(key);
  free(hash);

  // TODO: handle errors here properly
  // if 0, no bytes written, -1 is error and errno is set
  return (int) (write(server->client_fd, response, strlen(response)) == 0);
}

// Return 0 on success
int try_to_upgrade(ctrl_server *server)
  // Check we received a valid HTTP 101 upgrade to WS
  return (is_valid_http_upgrade(server)) ? upgrade_to_ws(server) : 1;
}

// Handle input from client, after input is read into buffer
// TODO: in real life this should take commands, and send back an in progress msg so webapp can block
int handle_input(ctrl_server *server) {
  for (int i = 0; i < 128; i++) {
    printf("%d: %08x ", i, server->buffer[i]);
  }
  printf("Buffer: \n%s", server->buffer); // TODO: fix seg fault here

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
  free(server);
  if (server->client_fd) {
    if (close(server->client_fd)) {
      perror("Closing client fd failed");
      exit(errno);
    }
  }
  if (close(server->socket_fd)) {
    perror("Closing socket fd failed");
    exit(errno);
  }
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
        handle_input(server);
      } else if (read_size == 0) {
        server->client_fd = 0;
      }
    } else {
      // Check if any client is waiting to connect
      server->client_fd = try_accept_conn(server);

      // If a connection was accepted, wait until it's upgraded it to a WebSocket
      if (server->client_fd) {
        // Block until we have an input from the connection. TODO: add timeout to avoid DoS
        ssize_t read_size;
        do {
          sleep_for(1); //TODO: make this shorter?
          read_size = get_latest_input(server);
        } while (read_size < 0);

        if (read_size > 0) {
          try_to_upgrade(server); //TODO: check a WS is actually created, and disconnect if not
        } else {
          server->client_fd = 0;
        }
      }

    }
    sleep_for(1);
  }

  close_server(server);
}







