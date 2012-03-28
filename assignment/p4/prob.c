/*
 *  Created by Maheshwar on 03/11/11.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

char buf[2048];

main ()
{
	int i;
	int p1[2],p2[2],p3[2],p4[2];
	pid_t ret;
	pipe (p1);
	pipe (p2);
	pipe (p3);
	pipe (p4);
	
	ret = fork ();

	if (ret == 0)
	{
		ret = fork();
		if(ret==0)
		{
			printf("in ls\n");
			close(1);
			dup(p2[1]);			//p2 is standerd out
			close(p2[0]);
			
			execlp ("ls", "ls", "-l", (char *) 0);
		}
		else
		{
			wait(NULL);
			
			printf("in uniq\n");
			close(0);
			dup2(p2[0],0);		//p2 is standerd in
			close (1);			//closing standerd out
			dup2 (p1[1],1);		//p1 is standerd out
			close (p1[0]);
			close(p2[1]);
			
			
			execlp ("ls", "ls","-l", (char *) 0);
		}
	}

	if (ret > 0)
	{
		wait (NULL);
		printf("in main\n");
		
		//p1 is used as input
		int rdsz = read(p1[0],buf,sizeof(buf));
		if(rdsz==-1)
		{
			printf("read error\n");
			return 0;
		}
		
		
		ret = fork();
		
		if(ret==0)
		{
			printf("in grep ^d\n");
			close(0);
			dup2(p3[0],0);
			
			write( p3[1],buf,sizeof(buf));
			
			execlp ("grep", "grep", "^-", (char *) 0);
		}
		else
		{
			wait(NULL);
			printf("in grep ^-\n");
			close(0);
			dup2(p4[0],0);
			close(1);
			dup2(p4[1],1);
			printf("%s",buf);
			
			execlp ("grep", "grep", "^-", (char *) 0);
		}
	
	}
		
}
