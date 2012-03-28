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
	
	if (argc!=2) {
		printf("Usage: %s <PortNumer>\n",argv[0]);
		exit(1);
	}
	int sock;
	struct sockaddr_in serverAddr;
	socklen_t serverAddrLen;
	sock = socket(PF_INET,SOCK_DGRAM,0);
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(atoi(argv[1]));
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddrLen = sizeof(serverAddr);
	
	bind(sock,(struct sockaddr*)&serverAddr,serverAddrLen);
	newSocket* clientSock;
	
	clientSock = acceptUDP(sock);
	
	char recvBuffer[100];
	
	if(recvFromUDP(clientSock,recvBuffer)==-1);	
	printf("Received: %s\n",recvBuffer);
	
	sendToUDP(clientSock,recvBuffer);
	sleep(10);
	
	return 0;
	
}
