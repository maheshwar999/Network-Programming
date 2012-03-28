/*
 *  Created by Praseem and Maheshwar on 05/11/11.
 *
 */
#include <netinet/in.h>

#ifndef FILL_ADDR_STRUCTURE
#define FILL_ADDR_STRUCTURE

void fillAddrStructure(struct sockaddr_in *serverAddr, unsigned short sin_port, char* sin_addr);

#endif
