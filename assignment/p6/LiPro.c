/*
 *  Created by Praseem and Maheshwar on 05/11/11.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include "dieWithError.h"
#include "fillAddrStruct.h"
#include "group.h"

group grpArr[100];
int grpLen;

int compareGroup(group g1, group g2){
	if (strcmp(g1.groupName,g2.groupName) == 0 && strcmp(g1.groupIp,g2.groupIp) == 0 && strcmp(g1.groupPort,g2.groupPort) == 0 ) {
		return 1;
	}
	else {
		return 0;
	}

	
	
}
group getGroup(){
	group g;
	printf("Enter the name\n");
	scanf("%s",g.groupName);
	printf("Enter the IP\n");
	scanf("%s",g.groupIp);
	printf("Enter the Port\n");
	scanf("%s",g.groupPort);
	return g;
	
}
int addGroup(group g){
	int i;	
	for (i=0; i<grpLen; i++) {
		if (compareGroup(grpArr[i],g)) {
			return 0;
		}
	}
	strcpy(grpArr[i].groupName,g.groupName);
	strcpy(grpArr[i].groupIp,g.groupIp);
	strcpy(grpArr[i].groupPort,g.groupPort);
	grpLen++;
	return 1;	
}

int removeGroup(group g){
	int i;
	for (i=0; i<grpLen; i++) {
		if (compareGroup(grpArr[i],g)) {
			strcpy(grpArr[i].groupName,grpArr[grpLen-1].groupName);
			strcpy(grpArr[i].groupIp,grpArr[grpLen-1].groupIp);
			strcpy(grpArr[i].groupPort,grpArr[grpLen-1].groupPort);
			strcpy(grpArr[grpLen-1].groupName, "");
			strcpy(grpArr[grpLen-1].groupIp, "");
			strcpy(grpArr[grpLen-1].groupPort, "");
			return 1;
		}
	}
	return 0;	
}

int modifyGroup(group oldG, group newG){
	int i;
	for (i=0; i<grpLen; i++) {
		if (compareGroup(grpArr[i],oldG)) {
			strcpy(grpArr[i].groupName,newG.groupName);
			strcpy(grpArr[i].groupIp,newG.groupIp);
			strcpy(grpArr[i].groupPort,newG.groupPort);
			return 1;
		}
	}
	return 0;	
}


int main(int argc, char** argv){
	int sockfd;
	struct sockaddr_in serverAddr;
	struct sockaddr_in replyAddr;
	char replyAddrPrint[20];
	socklen_t replyLen;
	char recvLine[100],sendLine[1000];
	int on = 1,n;
	fd_set rset, allset;
	int maxfd;
	int selection;
	char space[2] = {' ','\0'};
	char newLine[2] = {'\n','\0'};
	
	if (argc != 2) {
		dieWithError("Usage: Server <Port Number>\n");
	}
	
	memset(&serverAddr, 0, sizeof(serverAddr));
	fillAddrStructure(&serverAddr,atoi(argv[1]), NULL);
	
	
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd < 0)
		dieWithError("SockFd");
	
	
	n = bind(sockfd,(struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (n<0)
		dieWithError("Bind");
	
	//setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
	
	FD_ZERO(&allset);
	FD_SET(sockfd, &allset);
	FD_SET(STDIN_FILENO, &allset);
	
	maxfd = sockfd+1;
	if(fcntl(STDIN_FILENO,F_SETFL,O_NONBLOCK)<0)
		dieWithError("fcntl:");
		
	printf("Enter Selection\n1.Add new Group\n2.Delete a group\n3.Modify a group\n");
	//printf("Do what ever!!\n");
	while (1) {
		
		rset = allset;		
		fflush(stdout);
		select(maxfd,&rset,NULL,NULL,NULL);
		
		if (FD_ISSET(sockfd,&rset)) {
			memset(recvLine,0,100);
			replyLen = sizeof(replyAddr);
			n = recvfrom(sockfd, recvLine, 99, 0 , (struct sockaddr*)&replyAddr, &replyLen);
			recvLine[n-1] = '\0';
			inet_ntop(AF_INET,&(replyAddr.sin_addr),&replyAddrPrint,replyLen);
			printf("Msg: %s from: %s\n",recvLine,replyAddrPrint);
			int i;
			memset(sendLine,0,1000);
			for (i=0; i<grpLen; i++) {
				strcat(sendLine,grpArr[i].groupName);
				strcat(sendLine,space);
				strcat(sendLine,grpArr[i].groupIp);
				strcat(sendLine,space);
				strcat(sendLine,grpArr[i].groupPort);
				strcat(sendLine,newLine);
				//strcat(sendLine,'\n');
			}
			printf("Size of sendLine %d\n",strlen(sendLine));
			sendto(sockfd,sendLine,strlen(sendLine),0,(struct sockaddr*)&replyAddr,replyLen);			
		}
		else if(FD_ISSET(STDIN_FILENO,&rset)){
			//read(STDIN_FILENO,recvLine,100);
			
			//sscanf(recvLine,"%d",&selection);
			scanf("%d",&selection);
			if(fcntl(STDIN_FILENO,F_SETFL,0)<0)
				dieWithError("fcntl:");
			
			if (selection == 1) {
				if(!addGroup(getGroup()))
					printf("Group Already Present\n");
				else {
					printf("Group Added\n");
				}

			}
			else if(selection == 2){
				if(!removeGroup(getGroup()))
					printf("Group Not present\n");
				else {
					printf("Group Deleted\n");
				}				
			}
			else if(selection == 3){
				if(!modifyGroup(getGroup(),getGroup()))
					printf("Group Not present\n");
				else {
					printf("Group Modified\n");
				}				
			}
			else {
				printf("Wrong Choice Enter again");
				continue;
			}
			printf("Enter Selection\n1.Add new Group\n2.Delete a group\n3.Modify a group\n");
			if(fcntl(STDIN_FILENO,F_SETFL,O_NONBLOCK)<0)
				dieWithError("fcntl:");
			fflush(stdout);
		}
	}	
	return 0;
}

