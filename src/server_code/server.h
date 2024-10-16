#ifndef SERVER_H
#define SERVER_H

// Data about a client connected to the server
struct connectedClient {
  char username[USERNAME_SIZE];                 // Client username
  struct sockaddr_in socketUdpAddress;          // Address structure of connected client's UDP socket
  char availableResources[RESOURCE_ARRAY_SIZE]; // Filenames of files the client is willing to share
};

void shutdownServer(int);                       // Gracefully shutdown the server
int findEmptyConnectedClient(uint8_t);          // Find an empty connected client in an array of them
void printAllConnectedClients();                // Print out all connected clients. Debug purposes

#endif
