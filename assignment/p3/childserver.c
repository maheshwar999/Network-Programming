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
#include <string.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <assert.h>

#define MAX 2
#define MAXLINE 100
struct my_msgbuf
{
	int type;
	char text[100];
	char nick[20];
};

//global variables
int msqid;						//msg queue id
int client[MAX];				//client fd
char nick[MAX][20];				//nicks array

static void *receiver()
{
	int pid=getpid();
	struct my_msgbuf buf;
	pthread_detach(pthread_self());
	int n,i;
	while(1)
	{
		n=msgrcv(msqid, &(buf.type), sizeof (buf), pid, 0);
		for(i=0;i<MAX;i++)
		if(!strcmp(buf.nick,nick[i]))
		write(client[i],buf.text,strlen(buf.text));
		
	}
}


int main()
{
	//msg queue creation
	key_t key;
	if ((key = ftok ("mainserver.c", 'B')) == -1)
    {
      perror ("ftok");
      exit (1);
    }

  	if ((msqid = msgget (key, IPC_CREAT | 0644)) == -1)
    {
      perror ("msgget");
      exit (1);
    }
    
	pthread_t	tid;
	pthread_create(&tid, NULL, receiver, NULL);
	
	//creting sockets
	printf("my pid = %d\n",getpid());
	int i, maxfd, listenfd, connfd, sockfd;
	int nready,load=0;
	ssize_t n;
	fd_set rset, allset;
	char buf[MAXLINE];
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	listenfd = socket (AF_INET, SOCK_STREAM, 0);
	int on =1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,&on, sizeof(on) );
	//assert(listenfd!= -1);
	
	bzero (&servaddr, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	servaddr.sin_port = htons (6000);
	int a;
	a = bind(listenfd, (struct sockaddr *) & servaddr, sizeof (servaddr));
	
	while (a == -1){
		perror("Bind: ");
		printf("Bind Failed retrying!\n");
		fflush(stdout);
		sleep(10);		
		a = bind(listenfd, (struct sockaddr *) & servaddr, sizeof (servaddr));
	}
	printf("Bind Sucessful!\n");
	fflush(stdout);
	listen (listenfd,10);

	maxfd = listenfd;		/* initialize */
	memset(client,-1,sizeof(client));
	FD_ZERO (&allset);
	FD_SET (listenfd, &allset);

	for (;;)
	{
		rset = allset;		/* structure assignment */
		nready = select (maxfd + 1, &rset, NULL, NULL, NULL);
		
		if (FD_ISSET (listenfd, &rset))
		{			/* new client connection */
			
			//adding client details
			clilen = sizeof (cliaddr);
			connfd = accept (listenfd, (struct sockaddr *) & cliaddr, &clilen);
			printf ("new client: %s, port %d\n",inet_ntop (AF_INET, &cliaddr.sin_addr, 4, NULL),ntohs (cliaddr.sin_port));
			for (i = 0; i < MAX; i++)
				if (client[i] < 1)
				{
					client[i] = connfd;	/* save descriptor */
					break;
				}
				
			//load balancing
			load++;
			if(load==MAX)
			{
				FD_CLR (listenfd,&allset);
				FD_CLR (listenfd,&rset);
				close(listenfd);
				//printf("signal parent for forking\n");
				kill(getppid(), SIGUSR1);
			}

			//registering nick
			n = recv(connfd, buf, MAXLINE,0);
			buf[n-1]='\0';
			printf("%s",buf);
			
			char tnick[30];
			sscanf(buf,"nick : %s",tnick);
			strcpy(nick[i],tnick);
			struct my_msgbuf temp;
			temp.type=1;
			strcpy(temp.nick,tnick);
			sprintf(temp.text,"%d",getpid());
			
			//put into msg queue
			int a= msgsnd (msqid, &(temp.type), sizeof (temp), 0); 
			if(a<0) printf("msg queue error\n");
			
			
			FD_SET (connfd, &allset);	/* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd;	/* for select */

			if (--nready <= 0)
				continue;		/* no more readable descriptors */
		}

		for(i = 0; i <= MAX; i++)
		{			/* check all clients for data */
			if ((sockfd = client[i]) < 1)
				continue;
			if (FD_ISSET (sockfd, &rset))
			{
				n = recv (sockfd, buf, MAXLINE,0);
				
				if(n==0)
				{
					load--;
					struct my_msgbuf temp;
					temp.type=3;
					strcpy(temp.nick,nick[i]);
					strcpy(nick[i],"");
					sprintf(temp.text,"%d",getpid());
			
					//put into msg queue
					int a= msgsnd (msqid, &(temp.type), sizeof (temp), 0); 
					if(a<0) printf("msg queue error\n");
					
					kill(getpid(),SIGKILL);
					close (sockfd);
					FD_CLR (sockfd, &allset);
					client[i] = -1;
					continue;
				}
				buf[n]=0;
				//printf("%s",buf);
				char name[20],tmsg[100];
				sscanf(buf,"%s : %s",name,tmsg);
				sprintf(buf,"%s :: %s",nick[i],tmsg);
				int j;
				for(j=0;j<MAX;j++)
				if(client[j]!=-1 && strcmp(nick[j],name)==0)
				{
					int a=send(client[j],buf,strlen(buf),0);
					printf("from=%s  message=%s\n",nick[j],buf);
					break;
				}
				
				
				if(j==MAX)
				{	
					struct my_msgbuf temp;
					temp.type=2;
					strcpy(temp.nick,name);
					strcpy(temp.text,buf);
					msgsnd (msqid, &(temp.type), sizeof (temp), 0);
				}
				
				if (--nready <= 0)
					break;		/* no more readable descriptors */
			}
		}
	}
}


