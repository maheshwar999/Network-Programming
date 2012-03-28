/*
 *  Created by Maheshwar on 07/11/11.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>



int main(int argc, char *argv[])
{
	FILE *fp;
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;
	char buf[]="ji";
	int s;
	int len;
	int server_port=1234;
	
	hp=gethostbyname("127.0.0.1");
	if(!hp)
	{
		perror("enter correct server to connect");
		exit(0);
	}
	
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family=AF_INET;
	bcopy(hp->h_addr,(char *)&sin.sin_addr, hp->h_length);
	sin.sin_port=htons(server_port);
	
	if((s=socket(PF_INET,SOCK_STREAM,0))<0)
	{
		perror("error in making socket");
		exit(0);
	}
	
	if(connect(s, (struct sockaddr *)&sin,sizeof(sin)) < 0)
	{
		perror("error in connecting socket");
		close(s);
		exit(0);
	}
	
	int k=send(s,buf,strlen(buf),0);
	
	k=read(s,buf,sizeof(buf));
	buf[k]=0;
	printf("%s\n",buf);
	return 0;
}
	
	
