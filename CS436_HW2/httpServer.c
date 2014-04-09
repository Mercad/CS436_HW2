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
void Get(char reponse[], char request[]);
char* Head(char response[]);
void Delete(char request[], char response[]);
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
		"Error listening for connection request, errno = %d (%s) \n", errno,
		strerror(errno));

		return -1;
	}

	//wait for the client to send a request
	while (1)
	{
		clilen = sizeof(cliaddr);

		if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen))
				< 0)
		{
			if (errno== EINTR)
				continue;
			else
			{
				fprintf(stderr,
				"Error connection request refused, errno = %d (%s) \n", errno,
				strerror(errno));
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

		// Process the HTTP Request the user sent to the server.
		printf("The user sent the message: %s \n", buffer);
		if (contains(buffer, "HTTP/1.1") == 0 && contains(buffer, "HTTP/1.0")
				== 0)
		{
			printf("The message received was not an HTTP message %s", " ");
			close(fd);
			return;
		}

		char *str = requestHandler(request);

		write(fd, str, strlen(str));
	}
}

/********************************************************************************
 * requestHandler
 * ------------------------------------------------------------------------------
 * @param request the command issued from the client
 * @return response string to hold the contents of the command issued
 ********************************************************************************/
char *requestHandler(char request[])
{
	char* response = malloc(sizeof(char) * 9999);

	if (contains(request, "GET") == 1)
	{
		get(response, request);
		reponse = head(response);
	}
	else if (contains(buffer, "PUT") == 1)
	{
		put(request);
	}
	else if (contains(buffer, "DELETE") == 1)
	{
		delete(response, request);
	}
	else if (contains(buffer, "HEAD") == 1)
	{
		response = head(response);
	}
	else
	{
		strcpy(reponse, "Invalid Request\n");
	}

	return response;
}

void Get(char reponse[], char request[])
{
	char script[999];
	char directory[MAX_LINE];
	// Gets the filename from the sent request.
	char *fileName = (char*) malloc(sizeof(buffer) - 13);
	strncpy(fileName, buffer + 13, sizeof(buffer) - 13);
	fileName = substring(fileName, 0, strlen(fileName) - 2);
	FILE * fp;

	getcwd(directory, sizeof(directory));
	strcat(script, "cat ");
	strcat(script, directory);
	strcat(script, "/");
	strcat(script, fileName);
	printf("The script will run: %s. \n", script);
	// The script is now generated, just run it.

	fp = popen(script, "r");
	char line[999];
	while (fgets(line, sizeof(line) - 1, fp) != NULL)
	{
		strcat(response, line);
	}
	if (strlen(response) == 0)
	{
		strcat(response, "404 error: File not found");
	}
	printf("The response was: %s \n", response);
}

char* Head(char response[])
{
	char *headerResponse = malloc(sizeof(char) * 9999);
	char date[256];

	char *ctime();
	time_t now;

	(void) time(&now);
	sprintf(date, "%s", ctime(&now));

	strcat(headerResponse, "Content-Type: text/html; ");
	strcat(headerResponse, "charset=UTF-8 \n");
	strcat(headerResponse, "Server:CS436 Project \n");
	strcat(headerResponse, "Date: ");
	strcat(headerResponse, date);
	strcat(headerResponse, "Content-Length: ");
	strcat(headerResponse, strlen(response) + 3);
	strcat(headerResponse, "\n");
	strcat(headerResponse, response);
	strcat(headerResponse, "\n");

	return (headerResponse);
}

void Put(char request[])
{
	// Gets the filename from the sent request.
	char *temp = strstr(buffer, "<filedata>");
	int filenameOffset = temp - buffer;

	//File name is set
	char *fileName = (char*) malloc(filenameOffset - 1);
	strncpy(fileName, buffer + 13, filenameOffset - 14);

	printf("The file name is: %s\n", fileName);

	//Get the file Data
	char *fileData = (char*) malloc(sizeof(buffer) - filenameOffset);
	strncpy(fileData, buffer + filenameOffset,
			(sizeof(buffer) - filenameOffset) - 1);
	printf("The file data is: %s\n", fileData);

	FILE * fileOutput;
	fileOutput = fopen(fileName, "w");
	if (fileOutput != NULL)
	{
		fputs(fileData, fileOutput);
		fclose(fileOutput);
	}
	else
	{
		printf("Error Writing file\n");
	}
	return ("PUT request processed \n");
}

void Delete(char response[], char request[])
{
	char directory[MAX_LINE];
	// Gets the filename from the sent request.
	char *fileName = (char*) malloc(sizeof(request) - 16);
	strncpy(fileName, request + 16, sizeof(request) - 16);
	fileName = substring(fileName, 0, strlen(fileName) - 2);
	FILE * fp;
	getcwd(directory, sizeof(directory));
	strcat(script, "rm ");
	strcat(script, directory);
	strcat(script, "/");
	strcat(script, fileName);
	printf("The script will run: %s. \n", script);
	// The script is now generated, just run it.

	fp = popen(script, "r");
	char line[999];
	while (fgets(line, sizeof(line) - 1, fp) != NULL)
	{
		strcat(response, line);
	}
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
