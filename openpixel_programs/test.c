#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main() {
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

	int rval = connect(sd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	if (rval == -1) {
		printf("Port is closed\n");
		close(sd);
	} else {
		printf("Port is open\n");
	}
	
	send(sd, "message", strlen("hello"), 0);
	printf("sent hello\n");
	char buffer[1024] = {0};
	int valread = read(sd, buffer, 1024);

    return close(sd);
}
