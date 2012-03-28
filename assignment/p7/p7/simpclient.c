/*
 *  Created by Praseem on 09/11/11.
 *
 */
 
/* RPC client for simple addition example */

#include <stdio.h>
#include <stdlib.h>
#include "simp.h"  /* Created for us by rpcgen - has everything we need ! */

/* Wrapper function takes care of calling the RPC procedure */

char* lookup( CLIENT *clnt, int trainNo) {
	char *result;

	/* Call the client stub created by rpcgen */
	result = *(lookup_1(&trainNo,clnt));

	if (result==NULL) {
		fprintf(stderr,"Trouble calling remote procedure\n");
		exit(0);
	}
	return(result);
}

/* Wrapper function takes care of calling the RPC procedure */

char* booktkt( CLIENT *clnt, int trainNo) {
	char *result;

	/* Call the client stub created by rpcgen */
	result = *(booktkt_1(&trainNo,clnt));

	if (result==NULL) {
		fprintf(stderr,"Trouble calling remote procedure\n");
		exit(0);
	}
	return(result);
}


int main( int argc, char *argv[]) {
	CLIENT *clnt;
	int trainId,y;
	if (argc!=2) {
		fprintf(stderr,"Usage: %s hostname\n",argv[0]);
		exit(0);
	}

	/* Create a CLIENT data structure that reference the RPC
	   procedure SIMP_PROG, version SIMP_VERSION running on the
	   host specified by the 1st command line arg. */

	clnt = clnt_create(argv[1], SIMP_PROG, SIMP_VERSION, "udp");

	/* Make sure the create worked */
	if (clnt == (CLIENT *) NULL) {
		clnt_pcreateerror(argv[1]);
		exit(1);
	}

	/* get the 2 numbers that should be added */
	printf("Enter the trainNo for its details\n");
	scanf("%d",&trainId);
	char* res;
	res = lookup(clnt,trainId);
	printf("Train Info: %s\n",res);
	printf("Do you want to book a ticket\n1. Yes\n2. No\n");
	scanf("%d",&y);
	if(y==1){
		printf("Book Status: %s\n",booktkt(clnt,trainId)); 
	}else{
		printf("Bbye!!\n");
	}
	return(0);
}
