/*
 *  Created by Maheshwar on 10/11/11.
 *
 */
 
#include "trace.h"
#include <netdb.h>

struct proto proto_v4 = { NULL, recv_v4, NULL, NULL, NULL, NULL, 0,IPPROTO_ICMP, IPPROTO_IP, IP_TTL};

int     datalen = sizeof (struct rec); /* defaults */

int     max_ttl = 2;
int     nprobes = 3;
u_short dport = 32768 + 666;

int main(int argc, char **argv)
{
	int     c,n;
	struct addrinfo *ai,hints;
	struct hostent *hptr;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	//	hints.ai_socktype = SOCK_DGRAM;


	char   *h,s[INET_ADDRSTRLEN];
	char **pptr;
	host = argv [argc-1];
	if((hptr=gethostbyname(host))==NULL)
		perror("gethostbyname");

	pid = getpid();
	signal (SIGALRM, sig_alrm);

	pptr = hptr->h_addr_list;
	perror("assign");


	if(n=getaddrinfo(host,NULL,&hints,&ai)!=0)
		perror("error7");
	if(inet_ntop(AF_INET,*pptr,s,sizeof(s))==NULL)
		perror("error");

	printf ("traceroute to %s (%s) : %d hops max, %d data bytes\n",
	ai->ai_canonname ? ai->ai_canonname : s,s, max_ttl, datalen);
	
	printf("official hostname:%s\n",hptr->h_name);


	pr = &proto_v4;

	pr->sasend = ai->ai_addr; /* contains destination address */
	pr->sarecv = calloc (1, ai->ai_addrlen);
	pr->salast = calloc (1, ai->ai_addrlen);
	pr->sabind = calloc (1, ai->ai_addrlen);
	pr->salen = ai->ai_addrlen;

	traceloop();

	return (0);
}
