/*
 *  Created by Praseem and Maheshwar on 09/11/11.
 *
 */
#include "api.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

int main(int argc, char** argv){

	if (argc!=3) {
		printf("Usage: %s <Server IP> <PortNumer>\n",argv[0]);
		exit(1);
	}
	struct sockaddr_in serverAddr;
	socklen_t serverAddrLen;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET,argv[1],&serverAddr.sin_addr);
	serverAddrLen = sizeof(serverAddr);
	
	newSocket* server;
	server = connectUDP(&serverAddr,serverAddrLen);
	
	char buffer[1000] = "P r a s e e m";
	buffer[13] = '\0';
	char recv[1000];
	sendToUDP(server,buffer);
	recvFromUDP(server,recv);
	printf("Received %s\n",recv);

	return 0;
}


