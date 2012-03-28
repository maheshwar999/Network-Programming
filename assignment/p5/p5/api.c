/*
 *  Created by Praseem and Maheshwar on 09/11/11.
 *
 */
#include <sys/socket.h>
#include <time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "api.h"

int rto,ind;

newSocket* connections[100];

void addConnection(newSocket* con){
	connections[ind++] = con;
	//printf("Connection Added at %d\n",ind-1);
	return;	
}

void delConnection(newSocket* con){
	int i;
	for (i=0;i<ind; i++) {
		if (connections[i]==con) {
			connections[i] = connections[ind-1];
			connections[ind-1] = NULL;
			ind--;
			break;
		}
	}
}

void* handelIO(void* skt){
	//printf("Thread started!!\n");
	newSocket* sock = (newSocket*)skt;
	int i;
	int rto=3000000; //3 second intial sleep
	int srtt=0,delta,mrtt;
	int rttvar=750000; //0.75 second
	fcntl(sock->sock,F_SETFL,O_NONBLOCK);
	data* recv;
	data ack;
	time_t curTime;

	while (1) {
		time(&curTime);
		for (i=sock->start; i<sock->end; i++) {
			if (curTime > sock->nxtSendTime[i]) {
				time(&(sock->sendBuffer[i]->timestamp));
				//printf("\tSending Stiring %s %d\n",sock->sendBuffer[i]->buf,sock->sendBuffer[i]->len+2*sizeof(int)+sizeof(time_t));
				sendto(sock->sock,sock->sendBuffer[i],(sock->sendBuffer[i]->len+2*sizeof(int)+sizeof(time_t)+sizeof(char*)),0,(struct sockaddr*)&(sock->address),sock->addrlen);
				sock->nxtSendTime[i] = curTime + rto<<sock->count[i]; //exponential fall back next time to send
				sock->count[i]++;
				//printf("\tpacket send!!\n");
			}else {
				//printf("\tIn thread: curTime < nextSendTime for index %d\n",i);
			}
		}
		//printf("\tStart = %d, End = %d\n",sock->start,sock->end);

		//usleep(rto);
		recv = (data*)malloc(sizeof(data));
		time(&curTime);
		int n;
		while((n=recvfrom(sock->sock,recv,sizeof(data),0,NULL,NULL))!=-1){
			//printf("\tSome thing received\n");
			if (recv->len!=0) {
				sock->recvBuffer[(sock->recvBufInd)] = recv;
				sock->recvBufInd++;
				//printf("\trecvBufInd increased to %d\n",sock->recvBufInd++);
				ack.ackNo = recv->ackNo;
				ack.timestamp = recv->timestamp;
				ack.len = 0;
				sendto(sock->sock,(void*)&ack,8+sizeof(time_t),0,(struct sockaddr*)&(sock->address),sock->addrlen);
				//printf("\tAck Send\n");
			}else { //the recv is acknowedgement
				if(sock->ackNo[recv->ackNo] == 1){
					//ack has already been received 
					//printf("\tReAck Received\n");
				}else {
					//printf("\tAck Received for %d\n",recv->ackNo);
					sock->ackNo[recv->ackNo] = 1;
					mrtt = curTime - recv->timestamp;
					delta = mrtt - srtt;
					srtt += (delta/8);
					rttvar = rttvar +(abs(delta)-rttvar)/4;
					rto = srtt + 4*rttvar;
					printf("New RTO %d\n",rto);
				}			
			}
		}
		//printf("\tNothing More to receive\n");

		while (sock->ackNo[sock->start] == 1) {
			sock->start++;
		}		
	}	
}

newSocket*  acceptUDP(int sock){
	char buff[10];

	newSocket* clientAddr = (newSocket*)malloc(sizeof(newSocket));
	memset((void*)(clientAddr->count),0,50*sizeof(int));
	memset((void*)(clientAddr->nxtSendTime),0,50*sizeof(time_t));
	memset((void*)(clientAddr->ackNo),0,50*sizeof(int));
	clientAddr->start = 0;
	clientAddr->end = 0;
	clientAddr->nextAckNoToSend = 0;
	clientAddr->winLen = 4;
	clientAddr->recvBufInd = 0;
	clientAddr->addrlen = sizeof(clientAddr->address);

	int n = recvfrom(sock,buff,10,0,(struct sockaddr *)&(clientAddr->address),&(clientAddr->addrlen));
	if (n == -1) {
		perror("acceptUDP_recvfrom: ");
		return NULL;
	}
	//printf("Received: %s\n",buff);
	clientAddr->sock = socket(AF_INET,SOCK_DGRAM,0); //create a new socket for furthur communication
	if (clientAddr->sock == -1) {
		perror("socket: ");
		return NULL;
	}
	n = sendto(clientAddr->sock,buff,n,0,(struct sockaddr *)&(clientAddr->address),clientAddr->addrlen);
	if (n == -1) {
		perror("acceptUDP_sendto: ");
		return NULL;
	}
	addConnection(clientAddr);
	pthread_t pid;
	pthread_create(&pid,NULL,handelIO,(void*)clientAddr);
	return clientAddr;	
}

newSocket* connectUDP(struct sockaddr_in* serverAddr, socklen_t serverAddrLen){

	newSocket* clientAddr = (newSocket*)malloc(sizeof(newSocket));
	memset((void*)(clientAddr->count),0,50*sizeof(int));
	memset((void*)(clientAddr->nxtSendTime),0,50*sizeof(time_t));
	memset((void*)(clientAddr->ackNo),0,50*sizeof(int));
	clientAddr->addrlen = sizeof(clientAddr->address);
	clientAddr->start = 0;
	clientAddr->end = 0;
	clientAddr->nextAckNoToSend = 0;
	clientAddr->winLen = 4;
	clientAddr->recvBufInd = 0;
	clientAddr->sock = socket(AF_INET,SOCK_DGRAM,0);

	char buff[10] = "Connect";
	int n = sendto(clientAddr->sock,&buff,10,0,(struct sockaddr *)serverAddr,serverAddrLen);
	if (n == -1) {
		perror("connectUDP_sendto: ");
		return NULL;
	}

	n = recvfrom(clientAddr->sock,&buff,10,0,(struct sockaddr *)&(clientAddr->address),&(clientAddr->addrlen));
	if (n == -1) {
		perror("connectUDP_recvFrom: ");
		return NULL;
	}
	pthread_t pid;
	pthread_create(&pid,NULL,handelIO,(void*)clientAddr);
	addConnection(clientAddr);
	return clientAddr;

}

int recvFromUDP(newSocket* socket, char* buffer){
	struct sockaddr_in senderAddr;
	while (socket->recvBuffer[0]==NULL) {
		//printf("No messages received still\n");
		sleep(1);
	}
	//printf("Message Received!\n");
	strcpy(buffer,socket->recvBuffer[0]->buf);
	int i;
	for (i=0; i< socket->recvBufInd; i++)
		socket->recvBuffer[i] = socket->recvBuffer[i+1];
	socket->recvBufInd--;
	return 1;
}

int sendToUDP(newSocket* socket, char* buffer){

	data* sendData = (data*)malloc(sizeof(data));
	sendData->ackNo = socket->nextAckNoToSend;
	socket->nextAckNoToSend++;
	sendData->len = strlen(buffer);
	//printf("Lenth of data: %d\n",sendData->len);
	strcpy(sendData->buf,buffer);

	if (socket->end-socket->start < socket->winLen-1) {
		socket->sendBuffer[socket->end++] = sendData;
		//printf("Message added to sendBuffer at %d\n",socket->end-1);
	}else {
		//printf("Packet Dropped. Max window size achieved\n");
		return sendToUDP(socket,buffer);
	}	
	return 1;
}

