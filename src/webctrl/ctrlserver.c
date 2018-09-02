#include "ctrlserver.h"


static int first = 0;

// TODO: free up resources on exit failure

// TODO: handle requests properly

typedef struct {
  int socket_fd;
  int client_fd;
  char *buffer;
} ctrl_server;

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
  server->socket_fd = sockfd;
  server->client_fd = 0;
  server->buffer = buffer;

  return server;
}

// Accept a connection, if there are any waiting
int try_accept_conn(ctrl_server *server) {
  // Accept connection from an incoming client, if there are any waiting
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
  // TODO: find out why port is not displayed correctly
  printf("Client connected from host %s, port %hu\n",
         inet_ntoa(client_addr.sin_addr),
         ntohs (client_addr.sin_port));

  return client_sock;
}

// TODO: add error checking
char *extract_last_cmd(ctrl_server *server) {
  // Extract last command from buffer, ignore everything before
  char *last = strrchr(server->buffer, CMD_TERMINATOR);
  char *prev = server->buffer - 1;
  char *next = strchr(server->buffer, CMD_TERMINATOR);
  while (strcmp(next, last) != 0) {
    prev = next;
    next = strchr(next + 1, CMD_TERMINATOR);
  }
  return prev + 1;
}

// handle a HTTP upgrade to websocket request
// TODO: check how to identify this and code it
int handle_ws_upgrade(ctrl_server *server) {
  //printf("Message queue: \n%s", server->buffer);

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
  memcpy(key, key_start, key_len);
  memcpy(key + key_len, SEC_WEBSOCKET_MAGIC, strlen(SEC_WEBSOCKET_MAGIC));

  // TODO: decode from base64 first - doesn't seem like we need to
  unsigned char *hash = calloc(21, sizeof(char));
  SHA1((const unsigned char *)key, strlen((const char *) key), hash);

  // make sure null byte is set, TODO: use strncpy_s above instead??
  hash[20] = '\0';

  // base64 encode the output
  char *base64hash;
  Base64Encode(hash, strlen((const char *) hash), &base64hash);

  printf("hash: %s\n", base64hash);
  // TODO: need to create buffer and copy into
  char *response_start = "HTTP/1.1 101 Switching Protocols\r\n"
                         "Upgrade: websocket\r\n"
                         "Connection: Upgrade\r\n"
                         "Sec-WebSocket-Accept: ";
  char *response_end = "\r\n\r\n";
  char *response = calloc(sizeof(char), strlen(response_start) + strlen(response_end) + strlen(base64hash) + 1);
  strcat(response, response_start);
  strcat(response, base64hash);
  strcat(response, response_end);
  printf("%s", response);

  // TODO: fix error with extra character at end of base64 hash

  // TODO: handle errors here properly
  // if 0, no bytes written, -1 is error and errno is set
  return (int) (write(server->client_fd, response, strlen(response)) == 0);
}

// Handle input from client, after input is read into buffer
// TODO: in real life this should take commands, and send back an in progress msg so webapp can block
int handle_input(ctrl_server *server) {
  for (int i = 0; i < 128; i++) {
    printf("%d: %08x ", i, server->buffer[i]);
  }
  printf("Message queue: \n%s", server->buffer); // TODO: fix seg fault here

  // Get a pointer to the last command in the buffer
  char *last_cmd = extract_last_cmd(server);
  printf("Processing last cmd: %s", last_cmd);

  if (strncmp(last_cmd, "echo\n", 4) == 0) {
    return (int) write(server->client_fd, server->buffer, strlen(server->buffer));
  }

  return (int) write(server->client_fd, server->buffer, strlen(server->buffer));
}

// Get the latest input from the current connection
// Return 0 only if client is disconnected
int get_latest_input(ctrl_server *server) {
  ssize_t read_size;

  // Clear buffer
  for (int i = 0; i < BUFFER; i++) {
    server->buffer[i] = '\0';
  }

  // Get all waiting data from client in queue
  // TODO: check we don't get trapped here if data is continuous (DoS vulnerability?)
  // TODO: check we always get a null terminator, what happens if the while loop runs twice?
  while ((read_size = recv(server->client_fd, server->buffer, BUFFER, MSG_DONTWAIT)) > 0) {
    if (first == 0) {
      first = 1;
      if (handle_ws_upgrade(server)) {
        perror("Handle-input");
        exit(errno);
      }
    } else {
      if (handle_input(server)) {
        perror("Handle-input");
        exit(errno);
      }
    }
  }

  // No input is waiting
  if (errno == EAGAIN || errno == EWOULDBLOCK) {
    return 0;
  }

  // Client disconnected
  if (read_size == 0) {
    puts("Client disconnected");
    return 1;
  }

  // Other error
  if (read_size == -1) {
    perror("receive");
    exit(errno);
  }

  return 0;
}

// Cleanup code
int close_server(ctrl_server *server) {
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
  free(server->buffer);
  free(server);
  return 0;
}

// Set interrupted flag
int interrupted = 0;
void handle_user_exit(int _) {
  interrupted = 1;
}

// Sleep for s seconds
void sleep_for(uint8_t s) {
  struct timespec sleep;
  sleep.tv_nsec = 0;
  sleep.tv_sec = s;
  nanosleep(&sleep, NULL);
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
      // Check for input from client,
      if (get_latest_input(server)) {
        server->client_fd = 0;
      }
    } else {
      // Check if any client is waiting to connect
      server->client_fd = try_accept_conn(server);
    }
    sleep_for(1);
  }

  close_server(server);
}







