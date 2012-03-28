/*
 *  Created by Praseem and Maheshwar on 05/11/11.
 *
 */
#include <stdlib.h>
#include <netinet/in.h>

void fillAddrStructure(struct sockaddr_in *serverAddr, unsigned short sin_port, char* sin_addr){
	serverAddr->sin_family = AF_INET;
	serverAddr->sin_port = htons(sin_port);
	if(sin_addr == NULL)
		serverAddr->sin_addr.s_addr = INADDR_ANY;
	else 
		inet_pton(AF_INET,sin_addr,serverAddr->sin_addr);
	return;
}
