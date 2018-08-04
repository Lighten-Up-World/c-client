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

#define LISTEN_PORT "9999"
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
// TODO: add returns for client connected or no clients waiting
// TODO: make this non-blocking
int accept_conn(int sockfd) {

  // Accept connection from an incoming client, if there are any waiting
  struct sockaddr_in client_addr;
  size_t size = sizeof(client_addr);
  int client_sock = accept(sockfd, (struct sockaddr *) &client_addr, (socklen_t*) &size);

  if (client_sock < 0) {
    perror("accept");
    exit(errno);
  }
  printf("Client connected from host %s, port %hd\n",
         inet_ntoa(client_addr.sin_addr),
         ntohs (client_addr.sin_port));

  return client_sock;
}

// Get the latest input from the current connection
int get_latest_input(int client_sock) {
  ssize_t read_size;
  char client_message[2000];

  // Communicate with client until disconnected
  // TODO: add button on map to reset connection, go back to listening
  while ((read_size = recv(client_sock , client_message , 2000 , 0)) > 0 ) {
    // Send msg back to client
    write(client_sock , client_message , strlen(client_message));

    // Clear buffer
    for (int i = 0; i < 2000; i++) {
      client_message[i] = '\0';
    }
  }

  if (read_size == 0) {
    printf("Client disconnected\n");
  }
  else if(read_size == -1)
  {
    perror("receive");
    exit(errno);
  }

  return 0;
}

// Return true if a client is connected
int has_client(int sockfd) {
  char buffer[256];
  if (recv(sockfd, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0) {
    return 0;
  }
  return 1;
}

// Cleanup code
int close_socket(int sockfd) {
  return close(sockfd);
}

int interrupted = 0;

// Set interrupted flag
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

  while (!interrupted) {
    // Accept an incoming connection from client
    printf("Waiting for incoming connections...\n");
    int client;
    int i = 0;
    do {
      printf("try to accept: %d\n", i);
      client = accept_conn(sock);
      sleep_for(2);
      i++;
    } while (!has_client(client));

    // Get clients input
    get_latest_input(client);
  }

  close_socket(sock);
}







