/*
 *  Created by Praseem on 09/11/11.
 *
 */
 
/* Definition of the remote add and subtract procedure used by 
   simple RPC example 
   rpcgen will create a template for you that contains much of the code
   needed in this file is you give it the "-Ss" command line arg.
*/
#include <stdio.h>
#include "simp.h"
#include <stdlib.h>
/* Here is the actual remote procedure */
/* The return value of this procedure must be a pointer to int! */
/* we declare the variable result as static so we can return a 
   pointer to it */
train bookingInfo[100];

void initializeBooking(){
	int i;
	for(i=0;i<100;i++){
		bookingInfo[i].trainNo = i;
		bookingInfo[i].seatsAvlb = 100;
	}
}
int flag = 0;

char** lookup_1_svc(int *argp, struct svc_req *rqstp)
{

	static char result[1000];
	if(flag == 0){
		initializeBooking();
		flag =1;
	}
	printf("Got request: Lookup TrainNo. %d\n",*argp);
	int i = *argp;
	sprintf(result,"TrainNo: %d, SeatsAvlb: %d\n",bookingInfo[i].trainNo,bookingInfo[i].seatsAvlb);
	//printf("%s\n",result);
	static char *ret;
	ret = result;
	return (&ret);
}



char** booktkt_1_svc(int *argp, struct svc_req *rqstp)
{
	//static char* result = (char*)malloc(1000);
	static char result[1000];
	printf("Got request: BookTkt for TrainNo: %d\n",*argp);
	if(bookingInfo[*argp].seatsAvlb >= 1){
		bookingInfo[*argp].seatsAvlb--;
		sprintf(result,"Booking Successful!!\n");
	}
	else
		sprintf(result,"Booking Cannot be done. No seats Available!!\n");
	static char* ret = result;
	return (&ret);
}



