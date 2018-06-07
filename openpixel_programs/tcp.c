#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
  int sockid = socket(PF_INET, SOCK_STREAM, 0);

  struct sockaddr_in port_addr;

  port_addr.sin_family = AF_INET;
  port_addr.sin_port = htonl(7890);
  port_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int status = bind(sockid, (struct sockaddr *) &port_addr, sizeof(port_addr));

  printf("status: %d\n", status);

  return 0;
}
