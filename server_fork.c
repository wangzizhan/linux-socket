#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "wrap.h"

#define MAXLINE 80
#define SERV_PORT 8080

int main(void)
{
	struct sockaddr_in servaddr, cliaddr;
	socklen_t cliaddr_len;
	int listenfd, connfd;
	char buf[MAXLINE];
	char str[INET_ADDRSTRLEN];
	int i, n;
	int opt = 1;
	
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));	

	Bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	
	Listen(listenfd, 20);

	printf("Accepting connections ...\n");

	int m;
	while (1) {
		cliaddr_len = sizeof(cliaddr);
		connfd = Accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddr_len);
		
		m = fork();		
		if (m == -1) {
			perror("call to fork");
			exit(1);
		} else if (m == 0) {
			while (1) {	
				n = Read(connfd, buf, MAXLINE);
				if (n == 0) {
					printf("the other side has been closed.\n");
					break;
				}
				inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str));
				printf("received from %s at PORT %d\n", str, ntohs(cliaddr.sin_port));

				for (i = 0; i < n; i++) 
					buf[i] = toupper(buf[i]);
				Write(connfd, buf, n);
			}
			Close(connfd);
			exit(0);
		} else 
			Close(connfd);
	} 

	return 0;
}
