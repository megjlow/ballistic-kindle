#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include "request.h"
#include "firmata.h"

char** tokeniseString(char* input, char* separator) {
	char** retval;
	int count = 0;
	char* ptr = strdup(input);
	char* token = NULL;

	// count the number of tokens, exclude any lines which have only the token
	token = strsep(&ptr, separator);
	while(token != NULL) {
		if(strcmp(token, "") != 0) {
			count++;
		}
		token = strsep(&ptr, separator);
	}

	retval = calloc((count + 1), sizeof(char*));
	token = strsep(&input, separator);
	int i=0;
	while(i < count) {
	//for(i=0; i<count; i++) {
		if(strcmp(token, "") != 0) {
			retval[i] = calloc(strlen(token) + 1, sizeof(char));
			strcpy(retval[i], token);
			i++;
		}
		token = strsep(&input, separator);
	}
	// null terminate
	retval[count] = NULL;
	free(ptr);
	return retval;
}


void freeTokens(char** tokens) {
	for(int i=0; tokens[i] != NULL; i++) {
		free(tokens[i]);
	}
	free(tokens);
}



void processRequest(char* input) {
	char** header = tokeniseString(input, "\r\n");
	for(int i=0; header[i] != NULL; i++) {
		// find the get header, it's got /beetleaddress/pin/setting in it
		if(strncmp("GET", header[i], strlen("GET")) == 0) {
			char** url = tokeniseString(header[i], " ");
			if(url[1] != NULL) {
				char** cmd = tokeniseString(url[1], "/");
				// should have three tokens, beetleaddress pin setting
				if(cmd[0] != NULL)
				{
					// get the beetle address
					printf("beetle address %s\n", cmd[0]);
				}
				if(cmd[1] != NULL) {
					printf("pin number %s\n", cmd[1]);
				}
				if(cmd[2] != NULL) {
					printf("value %s\n", cmd[2]);
				}

				t_firmata     *firmata;

				firmata = firmata_new(cmd[0]);
				while(!firmata->isReady) { //Wait until device is up
				    firmata_pull(firmata);
				}
				firmata_pinMode(firmata, atoi(cmd[1]), MODE_OUTPUT);
				if(strcmp("1", cmd[2]) == 0) {
					firmata_digitalWrite(firmata, atoi(cmd[1]), HIGH);
				}
				else {
					firmata_digitalWrite(firmata, cmd[1], LOW);
				}
				firmata_end(firmata);

				freeTokens(cmd);
			}
			freeTokens(url);
		}
	}
	freeTokens(header);
}
