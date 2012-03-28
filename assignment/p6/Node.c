/*
 *  Created by Praseem and Maheshwar on 05/11/11.
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include "dieWithError.h"
#include "fillAddrStruct.h"
#include "group.h"


int main(int argc, char** argv){

	int sockfd,multicastfd,multicastfdRecv;
	struct sockaddr_in serverAddr;
	struct sockaddr_in multicastAddr;
	struct sockaddr_in multicastSendAddr;
	int serverLen,multicastSendAddrLen;
	
	char sendLine[1000] = "Send Group Info";
	char recvLine[1000];
	char senderIP[20];
	int on = 1;
	group grpArr[100];
	
	if (argc != 2) {
		dieWithError("Usage: Client <Server Port Number>\n");
	}
	
	memset(&serverAddr, 0, sizeof(serverAddr));
	fillAddrStructure(&serverAddr,atoi(argv[1]), NULL);
	serverAddr.sin_addr.s_addr = INADDR_BROADCAST;
	
	serverLen = sizeof(serverAddr);
	
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd < 0)
		dieWithError("SockFd");
	
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
	
	
	printf("Sending the request to server for Group information\n");
	int n = 15;
	n = sendto(sockfd, sendLine, n, 0 , (struct sockaddr *)&serverAddr, serverLen);
	if(n<0)
		dieWithError("send: ");
	
	
	n = recvfrom(sockfd, recvLine, 1000, 0 , NULL, NULL);
	
	char* itr1,*itr2,*itr3;
	itr1 = recvLine;
	int i=0;
	printf("Groups to join\n");
	while (itr1!=NULL) {
		itr2 = strsep(&itr1,"\n");
		if(strlen(itr2)<=0)
			break;
		printf("%d. %s\n",i+1,itr2);		
		itr3 = strsep(&itr2," ");
		strcpy(grpArr[i].groupName,itr3);
		itr3 = strsep(&itr2," ");
		strcpy(grpArr[i].groupIp,itr3);
		itr3 = strsep(&itr2," ");
		strcpy(grpArr[i].groupPort,itr3);
		i++;
	}
	printf("Enter the choise: ");
	scanf("%d",&n);
	n--;
	
	struct ip_mreq mreq;
	
	fillAddrStructure(&multicastAddr, atoi(grpArr[n].groupPort),NULL);
	multicastfd = socket(AF_INET,SOCK_DGRAM,0);
	if(multicastfd < 0)
		dieWithError("SockFd");
	
	//reciever
	multicastfdRecv = multicastfd;	
	bind(multicastfdRecv,(struct sockaddr*)&multicastAddr,sizeof(multicastAddr));
	mreq.imr_multiaddr.s_addr = inet_addr(grpArr[n].groupIp);         
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);  
	
	if (setsockopt(multicastfdRecv, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
		dieWithError("Setsockopt: ");
	}
	//------
	
	//sender
	multicastAddr.sin_addr.s_addr = inet_addr(grpArr[n].groupIp);
	//-----
	
	fd_set allset,rset,wset;
	int maxfd;
	
	FD_ZERO(&allset);
	FD_SET(STDIN_FILENO,&allset);
	FD_SET(multicastfdRecv,&allset);
	printf("Enter the message to send: ");
	fflush(stdout);
	//if(fcntl(STDIN_FILENO,F_SETFL,O_NONBLOCK)<0)
	//	dieWithError("fcntl:");
	maxfd = multicastfd+1;

	
	while (1) {
		rset = allset;
		select(maxfd,&rset,NULL,NULL,NULL);
		
		if (FD_ISSET(STDIN_FILENO,&rset)) {
			memset(sendLine,0,1000);
			n = read(STDIN_FILENO,sendLine,1000);
			sendLine[n-1]='\0';
			//printf("->>%s\n",sendLine);
			
			sendto(multicastfd,sendLine,n,0,(struct sockaddr*)&multicastAddr,sizeof(multicastAddr));
			//	perror("Sendto issue: ");
			printf("Enter the message to send: ");
			fflush(stdout);
		}else if (FD_ISSET(multicastfdRecv,&rset)) {
			multicastSendAddrLen = sizeof(multicastSendAddr);
			memset(recvLine,0,1000);			
			recvfrom(multicastfdRecv,recvLine,1000,0,(struct sockaddr*)&multicastSendAddr,&multicastSendAddrLen);
			//printf("->>%s\n",recvLine);
			inet_ntop(AF_INET,&(multicastSendAddr.sin_addr),&senderIP,multicastSendAddrLen);
			printf("Message-> %s :: from: %s\n",recvLine,senderIP);
			fflush(stdout);
		}
		
		
		
	}
	
	
}
