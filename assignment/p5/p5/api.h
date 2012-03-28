/*
 *  Created by Praseem and Maheshwar on 09/11/11.
 *
 */
#ifndef API_H
#define API_H
#include <time.h>
#include <netinet/in.h>

typedef struct{
	int ackNo;
	time_t timestamp;
	int len;
	char buf[1000];
}data;

typedef struct{	
	int sock;
	struct sockaddr_in address;
	socklen_t addrlen;
	data* sendBuffer[50];
	data* recvBuffer[50];
	int count[50];
	time_t nxtSendTime[50];
	int ackNo[50];
	int start;
	int end;
	int nextAckNoToSend;
	int winLen;
	int recvBufInd;
} newSocket;

extern newSocket*  acceptUDP(int sock);
extern int recvFromUDP(newSocket* socket, char* buffer);
extern int sendToUDP(newSocket* socket, char* buffer);
extern newSocket* connectUDP(struct sockaddr_in* serverAddr, socklen_t serverAddrLen);

#endif
