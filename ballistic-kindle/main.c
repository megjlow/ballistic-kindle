#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "client.h"

int main() {
	int serverSocket;
	int serverPort = 5000;
	struct sockaddr_in serv_addr;
	int err;

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(serverPort);

	err = bind(serverSocket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if(err < 0) {
		printf("failed to bind server socket on port %i\n", err);
	}

	err = listen(serverSocket, 10);

	// should now have a server socket listening on port 5000

	if(err < 0) {
		printf("listen failed %i\n", err);
	}

	while(1) {
		struct sockaddr_in client_addr;

		// wait for a connection
		socklen_t client_len = sizeof(client_addr);
		int client = accept(serverSocket, (struct sockaddr*)&client_addr, &client_len);

		if(fork() == 0) {
			// child process -
			processRequest(client, client_addr);
			exit(0);
		}
		else {
			// parent process, we're done here
			close(client);
		}

	}
}

/*
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>

int main()
{
int server_sockfd, client_sockfd;
int server_len, client_len;
struct sockaddr_in server_address;
struct sockaddr_in client_address;

server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = htonl(INADDR_ANY);
server_address.sin_port = htons(9734);
server_len = sizeof(server_address);
bind(server_sockfd, (struct sockaddr *)&server_address,server_len);

// Create a connection queue, ignore child exit details and wait for clients.

listen(server_sockfd, 5);

signal(SIGCHLD, SIG_IGN);

while(1) {
char ch;

printf("server waiting\n");

//Accept connection.

client_len = sizeof(client_address);
client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);

// Fork to create a process for this client and perform a test to see
//whether we're the parent or the child.

if(fork() == 0) {

// If we're the child, we can now read/write to the client on
//client_sockfd. The five second delay is just for this demonstration.

read(client_sockfd, &ch, 1);
sleep(5);
ch++;
write(client_sockfd, &ch, 1);
close(client_sockfd);
exit(0);
}

// Otherwise, we must be the parent and our work for this client is finished.

else {
close(client_sockfd);
}
}
}
 */
