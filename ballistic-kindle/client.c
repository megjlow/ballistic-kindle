#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "client.h"


int processRequest(int client_socket, struct sockaddr_in client_address) {
	printf("accepted connection from host %s, port %hd\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
	return 0;
}
