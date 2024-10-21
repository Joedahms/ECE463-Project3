#ifndef SERVER_H
#define SERVER_H

#define STATUS_SEND_INTERVAL 3000000            // Microseconds
#define MAX_CONNECTED_CLIENTS 100               // Maximum number of clients that can be connected to the server
#define MAX_RESOURCES 100

#include <stdbool.h>

// Data about a client connected to the server
struct ConnectedClient {
  char username[MAX_USERNAME];
  struct sockaddr_in socketUdpAddress;
//  char availableResources[MAX_RESOURCE_ARRAY];  // Filenames of files the client is willing to share
  bool status;                                  // If the client is connected or not
  bool requestedStatus;                         // If the server has sent a request asking if the client is still connnected
};

void* checkClientStatus();
void shutdownServer(int);
int findEmptyConnectedClient(bool);
void printAllConnectedClients();

int handleConnectionPacket(char*, struct sockaddr_in, bool);
int handleStatusPacket(struct sockaddr_in);
/*
int handleResourcePacket(struct sockaddr_in);
*/

#endif
