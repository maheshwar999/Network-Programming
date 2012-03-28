/*
 *  Created by Maheshwar on 07/11/11.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int argc, char *argv[]) {

  char *server = "127.0.0.1";     // First arg: server address/name
  char *echoString = "hi ra"; // Second arg: word to echo

  size_t echoStringLen = strlen(echoString);
  
  // Third arg (optional): server port/service
  char *servPort = "12345";

  // Tell the system what kind(s) of address info we want
  struct addrinfo addrCriteria;                   // Criteria for address match
  memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
  addrCriteria.ai_family = AF_UNSPEC;             // Any address family
  // For the following fields, a zero value means "don't care"
  addrCriteria.ai_socktype = SOCK_DGRAM;          // Only datagram sockets
  addrCriteria.ai_protocol = IPPROTO_UDP;         // Only UDP protocol

  // Get address(es)
  struct addrinfo *servAddr; // List of server addresses
  int rtnVal = getaddrinfo(server, servPort, &addrCriteria, &servAddr);
  

  // Create a datagram/UDP socket
  int sock = socket(servAddr->ai_family, servAddr->ai_socktype,servAddr->ai_protocol); // Socket descriptor for client
  

  // Send the string to the server
  ssize_t numBytes = sendto(sock, echoString, echoStringLen, 0,servAddr->ai_addr, servAddr->ai_addrlen);
  

  // Receive a response

  struct sockaddr_storage fromAddr; // Source address of server
  // Set length of from address structure (in-out parameter)
  socklen_t fromAddrLen = sizeof(fromAddr);
  char buffer[100]; // I/O buffer
  numBytes = recvfrom(sock, buffer, 100, 0,(struct sockaddr *) &fromAddr, &fromAddrLen);
 

  // Verify reception from expected source

  freeaddrinfo(servAddr);

  buffer[numBytes] = 0;     // Null-terminate received data
  printf("Received: %s\n", buffer); // Print the echoed string

  close(sock);
  exit(0);
}
