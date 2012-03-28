/*
 *  Created by Maheshwar on 07/11/11.
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
#include <netdb.h>
#include <stdbool.h>

int main()
{
	int i,j,k;
	char buff[100];
	
	j=read(0,buff,sizeof(buff));
	buff[j]=0;
	write(1,buff,strlen(buff));
	//sleep(100);
	return 0;
}

