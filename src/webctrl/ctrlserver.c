//#include "ctrlserver.h"

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>
#include <strings.h>
#include <stdio.h>
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

#define LISTEN_PORT "9998"
#define BUFFER_SIZE 1024
#define BACKLOG 0

// Set up a socket to listen on
int create_socket() {
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
    perror("socket--bind");
    exit(errno);
  }

  // Make it a "listening socket"
  if (listen(sockfd, BACKLOG)) {
    perror("socket--listen");
    exit(errno);
  }

  return sockfd;
}

// Accept a connection, if there are any waiting
int try_accept_conn(int sockfd) {
  // Accept connection from an incoming client, if there are any waiting
  struct sockaddr_in client_addr;
  size_t size = sizeof(client_addr);
  int client_sock = accept(sockfd, (struct sockaddr *) &client_addr, (socklen_t*) &size);

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
  printf("Client connected from host %s, port %hd\n",
         inet_ntoa(client_addr.sin_addr),
         ntohs (client_addr.sin_port));

  return client_sock;
}

// Get the latest input from the current connection
// Return 0 only if client is disconnected
int get_latest_input(int client_sock) {
  ssize_t read_size;
  char client_message[2000];

  // Clear buffer
  for (int i = 0; i < 2000; i++) {
    client_message[i] = '\0';
  }

  // Communicate with client until disconnected
  // TODO: add button on map to reset connection, go back to listening
  while ((read_size = recv(client_sock, client_message, 2000, 0)) > 0) {
    // Send msg back to client
    printf("Msg from client: %s", client_message);
    write(client_sock , client_message , strlen(client_message));

    // Clear buffer
    for (int i = 0; i < 2000; i++) {
      client_message[i] = '\0';
    }
  }

  // No input is waiting
  if (errno == EAGAIN || errno == EWOULDBLOCK) {
    return 1;
  }

  // Client disconnected
  if (read_size == 0) {
    puts("Client disconnected");
    return 0;
  }

  // Other error
  if (read_size == -1) {
    perror("receive");
    exit(errno);
  }

  return 1;
}

// Cleanup code
int close_socket(int sockfd) {
  return close(sockfd);
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
int main() {
  // Set up Ctrl+C handle
  signal(SIGINT, handle_user_exit);

  // Create socket
  int sock = create_socket();

  int client = 0;
  while (!interrupted) {
    // If client is connected
    if (client) {
      // Check for input from client
      if (!get_latest_input(client)) {
        client = 0;
      }
    } else {
      // Check if any client is waiting to connect
      client = try_accept_conn(sock);
    }
    sleep_for(1);
  }

  close_socket(sock);
}







