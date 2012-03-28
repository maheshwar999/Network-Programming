/*
 *  Created by Maheshwar on 01/11/11.
 *
 */
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <pthread.h>

void	*copyto(void *);

static int	sockfd;		/* global for both threads to access */
static FILE	*fp;

void str_cli(FILE *fp_arg, int sockfd_arg)
{
	char		recvline[1000];
	pthread_t	tid;

	sockfd = sockfd_arg;	/* copy arguments to externals */
	fp = fp_arg;
	pthread_create(&tid, NULL, copyto, NULL);
	int len;
	while ((len=read(sockfd, recvline, 1000)) > 0)
	{
		recvline[len]='\0';
		printf("reply-> %s\n",recvline);
	}
	//printf("ending strcli\n");
	
}

void *copyto(void *arg)
{
	char	sendline[1000];
    while (fgets(sendline, 1000, stdin) != NULL)
	write(sockfd, sendline, strlen(sendline));
	//printf("ending copyto\n");
	shutdown(sockfd, SHUT_WR);	/* EOF on stdin, send FIN */

	return(NULL);
}


int main(int argc,char **argv)
{
	if(argc!=2)
	{
		perror("usage : ./client <nick to be used>");
		exit(0);
	}
	int sockfd;
	struct sockaddr_in servaddr;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(6000);
	servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	if((connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)))<0)
	printf("connect error\n");
	char buff[100];
	sprintf(buff,"nick : %s\n",argv[1]);
	//printf("%s",buff);
	send(sockfd, buff, sizeof(buff),0);
	printf("message sent\n");
	str_cli(stdin,sockfd);
	exit(0);
}
