#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "../common/network_node.h"
#include "server.h"

// Global flags
uint8_t debugFlag = 0;  // Can add conditional statements with this flag to print out extra info

// Global variables (for signal handler)
int listeningUDPSocketDescriptor;

// Array of connected client data structures
struct connectedClient connectedClients[MAX_CONNECTED_CLIENTS];

// Main fucntion
int main(int argc, char* argv[]) {
  // Assign callback function for Ctrl-c
  signal(SIGINT, shutdownServer);

  // Make sure all connectedClients are set to 0
  int i; 
  for (i = 0; i < MAX_CONNECTED_CLIENTS; i++) {
    memset(&(connectedClients[i].socketUdpAddress), 0, sizeof(connectedClients[i].socketUdpAddress));
  }

  // Initialize socket address stuctures
  struct sockaddr_in serverAddress;                   // Socket address that clients should connect to
                                                      // Same port is used for both UDP and TCP connections
  struct sockaddr_in clientUDPAddress;                // Client's UDP info

  // Set up server sockaddr_in data structure
  memset(&serverAddress, 0, sizeof(serverAddress));   // 0 out
  serverAddress.sin_family = AF_INET;                 // IPV4
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);  
  serverAddress.sin_port = htons(PORT);               // Port
  
  checkCommandLineArguments(argc, argv, &debugFlag);  // Check if user passed any arguments

  setupUdpSocket(serverAddress);                      // Setup the UDP socket
  
  char* message = malloc(INITIAL_MESSAGE_SIZE);       // Space for incoming messages
  
  // Whether or not data is available at the socket. If it is, what kind.
  int udpStatus;

  // Continously listen for new UDP packets and new TCP connections
  while (1) {
    udpStatus = checkUdpSocket(listeningUDPSocketDescriptor, &clientUDPAddress, message, debugFlag);  // Check the UDP socket
    switch (udpStatus) {
      case 0:                                         // Nothing
      break;

      case 1:                                         // Something
      break;

        // This is here temporarily for reference later
        /*
        // Find the connected client with the sent TCP address and port
        // Then assign the UDP address and port it was sent to said connected client
        int i; 
        for (i = 0; i < MAX_CONNECTED_CLIENTS; i++) { // Loop through all connected clients
          unsigned long connectedClientAddress = ntohl(connectedClients[i].socketTcpAddress.sin_addr.s_addr); // Connected client address
          unsigned short connectedClientPort = ntohs(connectedClients[i].socketTcpAddress.sin_port);          // Connected client port

          // Does what was sent match the current connected client?
          if (tcpAddressInteger == connectedClientAddress && tcpPortInteger == connectedClientPort) {
            connectedClients[i].socketUdpAddress.sin_addr.s_addr = clientUDPAddress.sin_addr.s_addr;          // Set UDP address
            connectedClients[i].socketUdpAddress.sin_port = clientUDPAddress.sin_port;                        // Set UDP port
            break;                                                                                            // Don't loop through the rest of the connected clients
          }
        }
        if (debugFlag) {
          printAllConnectedClients(); 
        }
        break;  // Break case 1
*/
      default:  // Invalid message received
    }
  } // while(1)
  return 0;
} // main

/*
* Name: shutdownServer
* Purpose: Gracefully shutdown the server when the user enters
* ctrl-c. Closes the sockets and frees addrinfo data structure
* Input: The signal raised
* Output: None
*/
void shutdownServer(int signal) {
  close(listeningUDPSocketDescriptor);
  printf("\n");
  exit(0);
}

/*
 * Name: setupUdpSocket
 * Purpose: Setup the UDP socket. Set it to non blocking. Bind it. 
 * Input: Address structure to bind to.
 * Output: None
*/
void setupUdpSocket(struct sockaddr_in serverAddress) {
  // Set up UDP socket
  printf("Setting up UDP socket...\n");
  listeningUDPSocketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
  if (listeningUDPSocketDescriptor == -1) {
    perror("Error when setting up UDP socket");
    exit(1);
  }

  // Set non blocking
  int fcntlReturn = fcntl(listeningUDPSocketDescriptor, F_SETFL, O_NONBLOCK); // Set to non blocking
  if (fcntlReturn == -1) {
    perror("Error when setting UDP socket non blocking");
  }
  printf("UDP socket set up\n");

  // Bind UDP socket
  printf("Binding UDP socket...\n");
  int bindReturnUDP = bind(listeningUDPSocketDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress)); // Bind
  if (bindReturnUDP == -1) {
    perror("Error when binding UDP socket");
    exit(1);
  }
  printf("UDP socket bound\n");
}

/*
  * Name: findEmptyConnectedClient
  * Purpose: Loop through the connectedClients array until an empty spot is found. Looks for unset
  * UDP port.
  * Input: debugFlag
  * Output: Index of first empty spot
*/
int findEmptyConnectedClient(uint8_t debugFlag) {
  int connectedClientsIndex;
  for (connectedClientsIndex = 0; connectedClientsIndex < MAX_CONNECTED_CLIENTS; connectedClientsIndex++) { // Loop through all connected clients
    int port = ntohs(connectedClients[connectedClientsIndex].socketUdpAddress.sin_port);  // Check if the port has been set
    if (port == 0) {                        // Port not set
      return connectedClientsIndex;         // Empty spot, return index
      if (debugFlag) {
        printf("%d is empty\n", connectedClientsIndex);
      }
    }
    else {
      if (debugFlag) {
        printf("%d is not empty\n", connectedClientsIndex);
      }
    }
  }
  return -1;                                // All spots filled
}

/*
  * Name: printAllConnectedClients
  * Purpose: Print all the connected clients in a readable format
  * Input: None
  * Output: None
*/
void printAllConnectedClients() {
  printf("\n*** PRINTING ALL CONNECTED CLIENTS ***\n");
  int i;
  unsigned long udpAddress;
  unsigned short udpPort;
  for (i = 0; i < MAX_CONNECTED_CLIENTS; i++) {
    udpAddress = ntohl(connectedClients[i].socketUdpAddress.sin_addr.s_addr);
    udpPort = ntohs(connectedClients[i].socketUdpAddress.sin_port);
    if (udpAddress == 0 && udpPort == 0) {
      continue;
    }
    printf("CONNECTED CLIENT %d\n", i);
    printf("UDP ADDRESS: %ld\n", udpAddress);
    printf("UDP PORT: %d\n", udpPort);
  }
}
