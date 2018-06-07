#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main() {
	printf("running main\n");

    int sd;
    int port;
    int rval;
    struct hostent *hostaddr;
    struct sockaddr_in servaddr;

	sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sd == -1) {
		perror("Socket()\n");
		return (errno);
	}

	memset(&servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(7890);

	hostaddr = gethostbyname("127.0.0.1");
	memcpy(&servaddr.sin_addr, hostaddr->h_addr, hostaddr->h_length);

	rval = connect(sd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	if (rval == -1) {
		printf("Port %d is closed\n", port);
		close(sd);
	} else {
		printf("Port %d is open\n", port);
	}
	
	send(sd, "message", strlen("hello"), 0);
	printf("sent hello\n");
	char buffer[1024] = {0};
	int valread = read(sd, buffer, 1024);

    return close(sd);
}
