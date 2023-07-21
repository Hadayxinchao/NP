#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()

#define MAX 80
#define SA struct sockaddr

void func(int connfd)
{
    char buff[MAX];
    int n;
    int isLoggedIn = 0; // flag to check if a user is logged in

    for (;;) {
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        read(connfd, buff, sizeof(buff));

        if (strncmp("login", buff, 5) == 0) {
            // login command
            if (isLoggedIn) {
                write(connfd, "You are already logged in\n", sizeof("You are already logged in\n"));
            } else {
                // perform login logic here
                // validate user credentials and set isLoggedIn flag
                isLoggedIn = 1;
                write(connfd, "Login successful\n", sizeof("Login successful\n"));
            }
        } else if (strncmp("logout", buff, 6) == 0) {
            // logout command
            if (isLoggedIn) {
                // perform logout logic here
                isLoggedIn = 0;
                write(connfd, "Logout successful\n", sizeof("Logout successful\n"));
            } else {
                write(connfd, "You are not logged in\n", sizeof("You are not logged in\n"));
            }
        } else {
            write(connfd, "Invalid command\n", sizeof("Invalid command\n"));
        }
    }
}


int main(int argc, char *argv[])
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

     // Check if IP address and port number are provided
    if (argc < 3) {
        fprintf(stderr,"ERROR, no IP address or port provided\n");
        exit(1);
    }

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))!= 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else printf("connected to the server..\n");

	// function for chat
	func(sockfd);

	// close the socket
	close(sockfd);
}
