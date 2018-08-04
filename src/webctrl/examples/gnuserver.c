#include "gnuserver.h"
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>
#include <strings.h>

#define LISTEN_PORT 9999
#define MAXBUF 1024

int read_from_client(int filedes) {
  char buffer[MAXBUF];
  int nbytes;

  nbytes = (int) read(filedes, buffer, MAXBUF);
  if (nbytes < 0) {
    // Read error
    perror("read");
    exit(EXIT_FAILURE);
  } else if (nbytes == 0)
    // EOF
    return -1;
  else {
    // Data read
    fprintf(stderr, "Server: got message: `%s'\n", buffer);
    return 0;
  }
}

int main(int Count, char *Strings[]) {
  int sockfd;
  struct sockaddr_in self;
  char buffer[MAXBUF];

  // Create socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket");
    exit(errno);
  }

  // Init address struct
  //bzero(&self, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_port = htons(LISTEN_PORT);
  self.sin_addr.s_addr = INADDR_ANY;

  // Assign port to socket
  if (bind(sockfd, (struct sockaddr *) &self, sizeof(self))) {
    perror("socket--bind");
    exit(errno);
  }

  // Make it a "listening socket"
  // 20 is the backlog of connections to accept
  if (listen(sockfd, 20) != 0) {
    perror("socket--listen");
    exit(errno);
  }

  // Initialize the set of active sockets
  fd_set active_fd_set, read_fd_set;
  struct sockaddr_in client_addr;
  FD_ZERO (&active_fd_set);
  FD_SET (sockfd, &active_fd_set);
  while (1) {
    // Block until input arrives on one or more active sockets
    read_fd_set = active_fd_set;
    if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
      close(sockfd);
      perror("select");
      exit(EXIT_FAILURE);
    }

    // Service all the sockets with input pending
    for (int i = 0; i < FD_SETSIZE; ++i) {
      if (FD_ISSET (i, &read_fd_set)) {
        if (i == sockfd) {
          // Connection request on original socket
          int new;
          size_t size = sizeof(client_addr);
          new = accept(sockfd, (struct sockaddr *) &client_addr, (socklen_t *) &size);
          if (new < 0) {
            close(sockfd);
            perror("accept");
            exit(EXIT_FAILURE);
          }
          fprintf(stderr,
                  "Server: connect from host %s, port %hd.\n",
                  inet_ntoa(client_addr.sin_addr),
                  ntohs (client_addr.sin_port));
          FD_SET (new, &active_fd_set);
        } else {
          // Data arriving on an already-connected socket
          if (read_from_client(i) < 0) {
            close(i);
            FD_CLR (i, &active_fd_set);
          }
        }
      }
    }
  }

  // Never reached
  close(sockfd);
  return 0;
}