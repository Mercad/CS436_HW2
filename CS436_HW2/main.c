#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

#define MAXLINE 256
#define PORT_NUMBER 9001
const int backlog = 4;

void *clientHandler(void *arg);
char* substring(char *string, int position, int length);
int contains(char str1[], char str2[]);
//char *dnsLookup(char dns[]);

int main(int argc, char *argv[])
{
	// listen and connect socket
	int listenfd, connfd;
	pthread_t tid;
	int clilen;
	struct sockaddr_in cliaddr, servaddr;
	unsigned short portNumber;

	//get the port to wait on
	if (argc == 1)
	{
		portNumber = PORT_NUMBER;
	}
	else if (argc == 2)
	{
		portNumber = (unsigned short) atoi(args[1]);
	}
	else if (argc > 2)
	{
		printf("Usage: htmlServer <port> \n");
		return -1;
	}

	//create the socket
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1)
	{
		fprintf(stderr, "Error unable to create socket, errno = %d (%s) \n",
				errno, strerror(errno));
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(portNumber);

	//bind the socket
	if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1)
	{
		fprintf(stderr, "Error binding to socket, errno = %d (%s) \n", errno,
				strerror(errno));

		return -1;
	}

	//listen to the socket
	if (listen(listenfd, backlog) == -1)
	{
		fprintf(stderr,
				"Error listening for connection request, errno = %d (%s) \n",
				errno, strerror(errno));

		return -1;
	}

	//wait for the client to send a request
	while (1)
	{
		clilen = sizeof(cliaddr);

		if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen))
				< 0)
		{
			if (errno == EINTR)
				continue;
			else
			{
				fprintf(stderr,
						"Error connection request refused, errno = %d (%s) \n",
						errno, strerror(errno));
			}
		}

		//create a thread to handle the clients request
		if (pthread_create(&tid, NULL, clientHandler, (void *) &connfd) != 0)
		{
			fprintf(stderr,
					"Error unable to create thread, errno = %d (%s) \n", errno,
					strerror(errno));
		}

	}
}

/********************************************************************************
 * clientHandler
 * ------------------------------------------------------------------------------
 * @param arg the control of the server to read and write
 * @return
 ********************************************************************************/
void *clientHandler(void *arg)
{
	char request[MAXLINE];
	int i, n;

	int fd = *(int*) (arg);

	while (1)
	{
		if ((n = read(fd, request, MAXLINE)) == 0)
		{
			close(fd);
			return;
		}

		//char *str = dnsLookup(dns);

		write(fd, str, strlen(str));
	}
}

char *requestHandler(char request[])
{/*
	GET: Retrieve the specified resource
	HEAD: Asks for a response identical to the one that would correspond to a GET request, but without the response body. This is useful for retrieving meta-information written in response headers, without having to transport the entire content.
	PUT: Uploads the specified resource.
	DELETE: Deletes the specified resource.
	*/
}

char *substring(char *string, int position, int length)
{
	char *pointer;
	int index;
	pointer = malloc(length + 1);
	for (index = 0; index < (position - 1); index++)
	{
		string++;
	}

	for (index = 0; index < length; index++)
	{
		*(pointer + index) = *string;
		string++;
	}

	//null terminate the string
	*(pointer + index) = '\0';

	return pointer;
}

int contains(char str1[], char str2[])
{
	char* ptr;
	ptr = strstr(str1, str2);
	if (ptr == NULL)
	{
		return 0;
	}
	return 1;
}
