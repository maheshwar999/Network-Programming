/*
 *  Created by Maheshwar on 07/11/11.
 *
 */
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <netdb.h>
#include <stdbool.h>

int main()
{
	int i,j,k;
	char buff[100];

	struct sockaddr_storage clntAddr;
	socklen_t clntAddrLen = sizeof(clntAddr);
	int len=recvfrom(0, buff, 100, 0,(struct sockaddr *) &clntAddr, &clntAddrLen);
	//perror("recvFrom: ");
	buff[len-1]='\0';
	//printf("received : %s\n",buff);
	sendto(1, buff, len, 0,(struct sockaddr *) &clntAddr, clntAddrLen);
	//perror("Send to: ");
	sleep(1);

	return 0;
}

