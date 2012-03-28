/*
 *  Created by Maheshwar on 07/11/11.
 *
	the inetd.conf has lines like - "telnet stream tcp nowait root /usr/sbin/in.telnetd in.telnetd"
	<program> <socket_type> <protocol> <wait/nowait> <user> <path> <arguments>
	
	http://linuxmafia.com/pub/linux/suse-linux-internals/chapter11.html
	http://www.gnu.org/s/hello/manual/libc/Inetd-Servers.html
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
#include <assert.h>
#include <syslog.h>
#include <fcntl.h>
#include <signal.h>

#define LISTENQ 15
#define MAX 80

//typedef enum protocol{ tcp=1, udp=0};
typedef struct 
{
	int16_t port;
	int p; //tcp=1 or udp=0
	int concurrent; //yes or no
	char serverprogram_path[MAX]; //path of the executable
}service;

int listenfd[2], pids[2];
fd_set rset, allset;

void chld_handler(int signo)
{
	int j;
	int i=wait(&j);
	for(j=0;j<2;j++)
		if(i==pids[j])
		{
			pids[j]=-1;
			FD_SET (listenfd[j], &allset);
		}
}

int srtdaemon()
{
	int i;
	pid_t pid;
	if((pid=fork())<0)
		return (-1);
	else if(pid)
		_exit(0);
	if(setsid()<0)
		return (-1);
	signal(SIGHUP,SIG_IGN);
	if((pid=fork())<0)
		return (-1);
	else if(pid)
		_exit(0);
	for(i=0;i<64;i++)
		close(i);
	open("/dev/null",O_RDONLY);
	open("/dev/null",O_RDWR);
	open("/dev/null",O_RDWR);
	//openlog(pname,LOG_PID,0);
	return 0;
}

int main (int argc, char **argv)
{
	int a=srtdaemon();
	//printf(" pserver pid=%d\n",getpid());
	signal(SIGCHLD,chld_handler);
	int connfd, sockfd,k;
	int nready;
	ssize_t n;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	service arr[2];

	//filling the service array for storing the connection details
	arr[0].port=1234;
	arr[0].p = 1;
	arr[0].concurrent=1;
	strcpy(arr[0].serverprogram_path,"tcpserver");

	arr[1].port=12345;
	arr[1].p= 0;
	arr[1].concurrent=0;
	strcpy(arr[1].serverprogram_path,"udpserver");

	//initializing the tcp socket to accept connections at 1234port
	listenfd[0] = socket (AF_INET, SOCK_STREAM, 0);
	bzero (&servaddr, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	servaddr.sin_port = htons (1234);
	bind (listenfd[0], (struct sockaddr *) & servaddr, sizeof (servaddr));
	listen (listenfd[0], LISTENQ);

	//initializing udp socket to accept connections at 1235port
	char *service = "12345";
	struct addrinfo addrCriteria;                   // Criteria for address
	memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
	addrCriteria.ai_family = AF_UNSPEC;             // Any address family
	addrCriteria.ai_flags = AI_PASSIVE;             // Accept on any address/port
	addrCriteria.ai_socktype = SOCK_DGRAM;          // Only datagram socket
	addrCriteria.ai_protocol = IPPROTO_UDP;         // Only UDP socket
	struct addrinfo *servAddr; // List of server addresses
	int rtnVal = getaddrinfo(NULL,service, &addrCriteria, &servAddr);
	int sock = socket(servAddr->ai_family, servAddr->ai_socktype,servAddr->ai_protocol);
	assert(bind(sock, servAddr->ai_addr, servAddr->ai_addrlen)!=-1);
	listenfd[1]=sock;

	FD_ZERO (&allset);
	for(k=0;k<2;k++)
		FD_SET(listenfd[k], &allset);


	/* finding maxfd  */
	int j,maxfd=-1;		
	for(j=0;j<2;j++)
		if(maxfd<listenfd[j])
			maxfd=listenfd[j];
	
	memset(pids,-1,2*sizeof(int));

	for (;;)
	{
		int i,j,k;
		rset = allset;		/* structure assignment */
		//printf("Waiting at select\n");
		fflush(stdout);
		nready = select (maxfd + 1, &rset, NULL, NULL, NULL);

		for(i=0;i<2;i++)
		{
			//printf("For %d\n",i);
			if (FD_ISSET (listenfd[i], &rset))
			{			
				//printf("listenfd[%d] in rset\n",i);
				//printf("1 path= %s  pid=%d\n",arr[i].serverprogram_path,getpid());
				if(arr[i].concurrent)
				{
					//printf("Its concurrent\n");
					pids[i]=fork();
					if(!pids[i]) //child
					{
						//printf("In child TCP server\n");
						connfd=listenfd[i];
						if(arr[i].p)			//if tcp
							connfd = accept (listenfd[i], (struct sockaddr *) & cliaddr, &clilen);
						//printf("Connection Accepted\n");
						printf("path= %s  pid=%d\n",arr[i].serverprogram_path,getpid());
						close(0);
						close(1);
						close(2);
						dup2(connfd,0);
						dup2(connfd,1);
						dup2(connfd,2);
						int a = execl(arr[i].serverprogram_path,arr[i].serverprogram_path,(char*) 0);
						if(a == -1)
							perror("execl: ");
					}
					sleep(3);

				}
				else
				{
					//printf("Its not concurrent\n");
					FD_CLR(listenfd[i], &allset);		//removing from allset
					//printf("Removing form allset %d\n",i);
					FD_CLR(listenfd[i], &rset);
					pids[i]=fork();
					if(pids[i]==0)
					{
						connfd=listenfd[i];
						if(arr[i].p)			//if tcp
							connfd = accept (listenfd[i], (struct sockaddr *) & cliaddr, &clilen);

						printf("path= %s  pid=%d\n",arr[i].serverprogram_path,getpid());
						close(0);
						close(1);
						close(2);
						dup2(connfd,0);
						dup2(connfd,1);
						dup2(connfd,2);
						int a  = execl(arr[i].serverprogram_path,arr[i].serverprogram_path,(char*) 0);	
						if(a == -1)
							perror("execl: ");

					}
					sleep(3);
				}
			}
		}
	}
}

