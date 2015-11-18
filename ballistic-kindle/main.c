#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>


#include "request.h"


void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}


int main() {
	//key_t shMemId;
	int serverSocket;
	int serverPort = 5000;
	struct sockaddr_in serv_addr;
	int err;
	/*
	socket_t *sockets;

	// bluetooth socket storage in shared memory, for the moment assign enough memory for 10
	shMemId = shmget(shMemKey, 10 * sizeof(socket_t), IPC_CREAT | 0666);
	if(shMemId < 0) {
		// oops
	}
	int* shm = shmat(shMemId, NULL, 0);
	sockets = (struct socket_t*)shm;
	*/
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
			//processRequest(client, client_addr);
			//clientWorker(client);
			char buffer[8192];
			int nbytes = read(client, buffer, 8191);

			if(nbytes > 0) {
				//char input[8192];
				//strncpy(input, buffer, nbytes);
				processRequest(buffer);
				close(client);
			}

			exit(0);
		}
		else {
			// parent process, we're done here
			close(client);
		}

	}

}
