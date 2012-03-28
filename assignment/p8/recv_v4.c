/*
 *  Created by Maheshwar on 10/11/11.
 *
 */
 
#include "trace.h"

extern int gotalarm;

/*
   4  * Return: -3 on timeout
   5  *         -2 on ICMP time exceeded in transit (caller keeps going)
   6  *         -1 on ICMP port unreachable (caller is done)
   7  *       >= 0 return value is some other ICMP unreachable code
   8  */

int recv_v4(int seq, struct timeval *tv)
{
	int    k,hlen1, hlen2, icmplen, ret;
	socklen_t len;
	ssize_t n;
	struct ip *ip, *hip;
	struct icmp *icmp;
	struct udphdr *udp;
	char r[BUFSIZE];
	signal(SIGALRM,sig_alrm);             
	gotalarm = 0;
	alarm(3);
	
	for ( ; ; ) 
	{
		if (gotalarm)
			return -3;        /* alarm expried */
			
		len = pr->salen;
		
		n = recvfrom (recvfd, recvbuf,BUFSIZE, 0, pr->sarecv, &len); 
		if (n < 0) 
		{
			if (errno == EINTR)
			{	               
				printf("debug statement-one");
				continue;
			}
			else
			{
				printf ("recvfrom error");
				break;
			}
		}

		ip = (struct ip *) recvbuf; /* start of IP header */
		hlen1 = ip->ip_hl << 2; /* length of IP header */
		icmp = (struct icmp *) (recvbuf + hlen1); /* start of ICMP header */
		
		if ( (icmplen = n - hlen1) < 8)
		{
			printf("debug statement-two");
			continue;   
		}  /* not enough to look at ICMP header */

		if (icmp->icmp_type == ICMP_TIMXCEED && icmp->icmp_code == ICMP_TIMXCEED_INTRANS) 
		{
			if (icmplen < 8 + sizeof (struct ip))
			{   
				continue;         /* not enough data to look at inner IP */
				printf("debug statement-three");
			}
			
			hip = (struct ip *) (recvbuf + hlen1 + 8);
			hlen2 = hip->ip_hl << 2;
			
			if (icmplen < 8 + hlen2 + 4)
			{   
				continue;        /* not enough data to look at UDP ports */
				printf("debug statement-four");
			}
			
			udp = (struct udphdr *) (recvbuf + hlen1 + 8 + hlen2);
			
			if (hip->ip_p == IPPROTO_UDP && udp->source == htons (sport) &&udp->dest == htons (dport + seq)) 
			{
				ret = -2;        /* we hit an intermediate router */
				break;
			}

		} 
		
		else if (icmp->icmp_type == ICMP_UNREACH) 
		{
			if (icmplen <8 + sizeof (struct ip))
			{         
				continue;       /* not enough data to look at inner IP */
				printf("five");
			}
			
			hip = (struct ip *) (recvbuf + hlen1 + 8);
			hlen2 = hip->ip_hl << 2;
			
			if (icmplen < 8 + hlen2 + 4)
			{        
				continue;       /* not enough data to look at UDP ports */
				printf("six");
			}
			
			udp = (struct udphdr *) (recvbuf + hlen1 + 8 + hlen2);
			
			if (hip->ip_p == IPPROTO_UDP && udp->source == htons (sport) &&udp->dest == htons (dport + seq)) 
			{
				if (icmp->icmp_code == ICMP_UNREACH_PORT)
					ret = -1;    /* have reached destination */
				else
					ret = icmp->icmp_code; /* 0, 1, 2, ... */
				break;
			}
		}
		
		inet_ntop(AF_INET,pr->sarecv,r,BUFSIZE);   
		printf (" (from %s: type = %d, code = %d)\n",r,icmp->icmp_type, icmp->icmp_code);
		/* Some other ICMP error, recvfrom() again */
	}
	
	alarm(0);                   /* don't leave alarm running */
	gettimeofday (tv, NULL);    /* get time of packet arrival */
	return (ret);
}
