#ifndef SERVER_H
#define SERVER_H

#define STATUS_SEND_INTERVAL 3000000
#define MAX_CONNECTED_CLIENTS 100               // Maximum number of clients that can be connected to the server

#include <stdbool.h>

// Data about a client connected to the server
struct ConnectedClient {
  char username[USERNAME_SIZE];                 // Client username
  struct sockaddr_in socketUdpAddress;          // Address structure of connected client's UDP socket
  char availableResources[RESOURCE_ARRAY_SIZE]; // Filenames of files the client is willing to share
  bool status;
  bool requestedStatus;
};

void* checkClientStatus();
void shutdownServer(int);                       // Gracefully shutdown the server
int findEmptyConnectedClient(bool);          // Find an empty connected client in an array of them
void printAllConnectedClients();                // Print out all connected clients. Debug purposes

int handleConnectionPacket(char*, struct sockaddr_in, bool);
int handleStatusPacket(struct sockaddr_in);
int handleResourcePacket(struct sockaddr_in);

#endif
