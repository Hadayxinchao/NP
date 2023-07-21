// inet_addr
#include <arpa/inet.h>
// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()

#define MAX_ATTEMPTS 3
#define MAX 80
#define SA struct sockaddr

struct Account {
	char userID[30];
	char password[30];
	int status;
};

// Function designed for chat between client and server.
// Function designed for chat between client and server.
// Function designed for chat between client and server.
void func(int connfd)
{
    char buff[MAX];
    int n;
    struct Account account;
    int loginAttempts = 0;
    int isUserLoggedIn = 0; // flag to check if a user is logged in

    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        n = read(connfd, buff, sizeof(buff));
		if (n == 0) {
            // Connection closed by client
            printf("Client terminated the connection\n");
            break;
        }
        else if (n < 0) {
            // Error reading from socket
            perror("Error reading from socket");
            break;
        }
        printf("Received message from client: %s\n", buff);
		
        if (strncmp("login", buff, 5) == 0) {
            // login command
            char userID[30], password[30];
            sscanf(buff, "login %s %s", userID, password);
            printf("User ID: %s\n", userID);
            printf("Password: %s\n", password);

            FILE *file = fopen("account.txt", "r+");
            if (file == NULL) {
                printf("Cannot open account file\n");
                exit(1);
            }

            int found = 0;
            while (fscanf(file, "%s %s %d", account.userID, account.password, &account.status) == 3) {
                if (strcmp(account.userID, userID) == 0) {
                    found = 1;
                    printf("User ID found: %s\n", account.userID);
                    printf("Account Status: %d\n", account.status);

                    if (account.status == 0) {
                        write(connfd, "Account is locked\n", sizeof("Account is locked\n"));
                        printf("Account is locked\n");
                    } else if (strcmp(account.password, password) == 0) {
                        write(connfd, "Login successful\n", sizeof("Login successful\n"));
                        printf("Login successful\n");
                        loginAttempts = 0;
                        isUserLoggedIn = 1; // user is now logged in
                    } else {
                        loginAttempts++;
                        if (loginAttempts >= MAX_ATTEMPTS) {
                            account.status = 0;
                            write(connfd, "Account is locked due to too many login attempts\n", sizeof("Account is locked due to too many login attempts\n"));
                            printf("Account is locked due to too many login attempts\n");
                            // update account status in file
                            fseek(file, -sizeof(struct Account), SEEK_CUR);
                            fprintf(file, "%s %s %d\n", account.userID, account.password, account.status);
                        } else {
                            write(connfd, "Invalid password. Please try again\n", sizeof("Invalid password. Please try again\n"));
                            printf("Invalid password. Please try again\n");
                        }
                    }
                    break;
                }
            }

            if (!found) {
                write(connfd, "Invalid user ID. Please try again\n", sizeof("Invalid user ID. Please try again\n"));
                printf("Invalid user ID. Please try again\n");
            }

            fclose(file);
        } else if (strncmp("logout", buff, 6) == 0) {
            // logout command
            if (isUserLoggedIn) {
                write(connfd, "Logout successful\n", sizeof("Logout successful\n"));
                printf("Logout successful\n");
                isUserLoggedIn = 0; // user is now logged out
            } else {
                write(connfd, "You are not logged in\n", sizeof("You are not logged in\n"));
                printf("You are not logged in\n");
            }
        }
		write(connfd, "Server response", strlen("Server response") + 1);
    }
	close(connfd);
}


// Driver function
int main(int argc, char *argv[])
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// Check if port number is provided
	if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
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
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accept the client...\n");

	// Function for chatting between client and server
	func(connfd);

	// After chatting close the socket
	close(sockfd);
}
