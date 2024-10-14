#ifndef SERVER_H
#define SERVER_H

// Data about a client connected to the server
struct connectedClient {
  struct sockaddr_in socketUdpAddress;  // Address structure of connected client's UDP socket
};

void shutdownServer(int);                                 // Gracefully shutdown the server
void setupUdpSocket(struct sockaddr_in);                  // Setup a UDP socket
int findEmptyConnectedClient(uint8_t);                    // Find an empty connectedClient in an array of them
void printAllConnectedClients();                          // Print out all connected clients. Debug purposes

#endif
