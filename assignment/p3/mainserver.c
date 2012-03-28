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

#define MAX 100
struct my_msgbuf
{
	int type;
	char text[100];
	char nick[20];
};

int clients[100];
char nicks[100][20];
int msqid;

void  chld_handler(int signo)
{
	int i,j;
	i=wait(&j);
}
void int_handler(int signo)
{
  	int t=fork();
  	if(t==0)
  	execl("childserver","childserver",(char *)0);
}
void *remover()
{
	struct my_msgbuf buf;
	char nck[20];
	pthread_detach(pthread_self());
	while(1)
	{
		int n=msgrcv(msqid, &(buf.type), sizeof (buf), 3, 0);	
		//printf("removing msg of type 1\n");
		sscanf(buf.nick,"%s",nck);
		
		for(n=0;n<MAX;n++)
		if(strcmp(nck,nicks[n])==0)
		{
			strcpy(nicks[n],"");
			clients[n]=-1;
			break;
		}
	}
}
void *registerer()
{
	struct my_msgbuf buf;
	char nck[20];
	int clt;
	pthread_detach(pthread_self());
	while(1)
	{
		int n=msgrcv(msqid, &(buf.type), sizeof (buf), 1, 0);	
		//printf("removing msg of type 1\n");
		sscanf(buf.nick,"%s",nck);
		sscanf(buf.text,"%d",&clt);
		
		for(n=0;n<MAX;n++)
		if(clients[n]==-1)
		{
			strcpy(nicks[n],nck);
			clients[n]=clt;
			break;
		}
	}
}

int main()
{
	signal (SIGUSR1, int_handler);
	signal (SIGCHLD, chld_handler);
	memset(clients ,-1, sizeof(clients));
	
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
    
    pthread_t	tid,tid2;
	pthread_create(&tid, NULL, registerer, NULL);
	pthread_create(&tid2,NULL, remover , NULL);
	kill(getpid(), SIGUSR1);

	struct my_msgbuf buf;
	while(1)
	{
		int n=msgrcv(msqid, &(buf.type), sizeof (buf), 2, 0);	
		//printf("removing msg of type 2\n");
		for(n=0;n<MAX;n++)
		if(!strcmp(buf.nick,nicks[n]))
		{
			buf.type=clients[n];	
			msgsnd (msqid, &(buf.type), sizeof (buf), 0); 
			break;
		}
		
		if(n==MAX)
		{
			char temp1[50],sdtext[100];
			sscanf(buf.text,"%s : %s",temp1,sdtext);
			sprintf(buf.text,"%s is not connected\n",buf.nick);
			strcpy(buf.nick,temp1);
			int j;
			for(j=0;j<MAX;j++)
			if(strcmp(temp1,nicks[j])==0)
			{
				buf.type=clients[j];
				msgsnd (msqid, &(buf.type), sizeof (buf), 0); 
			}
		}
			
	}
	
	return 0;
}
