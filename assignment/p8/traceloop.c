/*
 *  Created by Maheshwar on 10/11/11.
 *
 */
 
#include "trace.h"
#include <netinet/in.h>

void tv_sub (struct timeval *out, struct timeval *in)
{
	if ((out->tv_usec -= in->tv_usec) < 0) 
	{     /* out -= in */
		--out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}

void traceloop(void)
{
	int     seq, code, done;
	double  rtt;
	struct rec *rec;
	struct timeval tvrecv;
	struct sockaddr_in *ptr1,*ptr2;
	recvfd = socket(AF_INET, SOCK_RAW,IPPROTO_ICMP);

	setuid (getuid());           /* don't need special permissions anymore */
	
	sendfd = socket (pr->sasend->sa_family, SOCK_DGRAM,0);
	//   printf("%d\t\n",sendfd);
	
	pr->sabind->sa_family = pr->sasend->sa_family;
	
	sport = getpid();     /* our source UDP port # */
	
	//sock_set_port (pr->sabind, pr->salen, htons (sport));
	ptr1 = (struct sockaddr_in *)pr->sabind;
	ptr1->sin_port=htons(sport);

	bind (sendfd, pr->sabind, pr->salen);

	//signal(SIGALRM,sig_alrm);	  
	sig_alrm (SIGALRM);
	seq = 0;
	done = 0;
	for (ttl = 1; ttl <= max_ttl && done == 0; ttl++) 
	{
		setsockopt (sendfd, pr->ttllevel, pr->ttloptname, &ttl, sizeof (int));
		bzero (pr->salast, pr->salen);
		printf ("%2d ", ttl);
		fflush (stdout);

		for (probe = 0; probe < nprobes; probe++) 
		{
			rec = (struct rec *) sendbuf;
			rec->rec_seq = ++seq;
			rec->rec_ttl = ttl;
			gettimeofday (&rec->rec_tv, NULL);
			ptr2 = (struct sockaddr_in *)pr->sasend;
			ptr2->sin_port=htons(dport + seq);
			

			// sock_set_port (pr->sasend, pr->salen, htons (dport + seq));
			sendto (sendfd, sendbuf, datalen, 0, pr->sasend, pr->salen);
			
			if ( (code = recv_v4(seq, &tvrecv)) == -3)
				printf ("**"); /* timeout, no reply */
			else 
			{
				char st [BUFSIZE],b[BUFSIZE];

				if (probe == 0) 
				{
					inet_ntop(AF_INET,pr->sarecv,b,BUFSIZE);

					if (getnameinfo (pr->sarecv, pr->salen, st, sizeof (st),NULL, 0, 0) == 0)
						printf (" %s (%s)", st,b );
								// Sock_ntop_host (pr->sarecv, pr->salen)		
					else
						printf (" %s",b );// Sock_ntop_host (pr->sarecv, pr->salen)
					memcpy (pr->salast, pr->sarecv, pr->salen);
				}
				
				tv_sub (&tvrecv, &rec->rec_tv);
				rtt = tvrecv.tv_sec * 1000.0 + tvrecv.tv_usec / 1000.0;
				printf (" %.3f ms", rtt);
				if (code == -1) /* port unreachable; at destination */
				{  
					done++;
					printf("done");
				}
				else if (code >= 0)
					printf (" (ICMP %s)", (*pr->icmpcode) (code));
			}
			fflush (stdout);
		}
		printf ("\n");
	}
}
