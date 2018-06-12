#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <netdb.h>
#include <unistd.h>

// Returns -1 on failure of connect()
int open_pixel_connection() {
  /*int sock_id = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock_id != 0) {
    perror("Creating socket failed\n");
    return errno;
  }

  struct sockaddr_in port_addr;
  port_addr.sin_family = AF_INET;
  port_addr.sin_port = (in_port_t) htonl(7890);
  port_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  return bind(sock_id, (struct sockaddr *) &port_addr, sizeof(port_addr));*/

  struct sockaddr_in servaddr;

  int sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(sd == -1) {
    perror("Socket()\n");
    return (errno);
  }

  memset(&servaddr, 0, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(7890);

  struct hostent *hostaddr = gethostbyname("127.0.0.1");
  memcpy(&servaddr.sin_addr, hostaddr->h_addr, hostaddr->h_length);

  return connect(sd, (struct sockaddr *) &servaddr, sizeof(servaddr));
}

// Returns -1 on failure of shutdown()
int close_pixel_connection(int sockid) {
  return shutdown(sockid, SHUT_RDWR);
}

int main() {
  int sock = open_pixel_connection();
  printf("open status: %d\n", sock);

  //int count = send(sockid, msg, msgLen, flags);

  //printf("close status: %d\n", close_pixel_connection(sock));

/*
    int count = send(sockid, msg, msgLen, flags);   msg: const void[], message to be transmitted
    msgLen: integer, length of message (in bytes) to transmit   flags: integer, special options, usually just 0
    count: # bytes transmitted (-1 if error)
*/
  return 0;
}
