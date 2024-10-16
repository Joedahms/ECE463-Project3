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
#include "../common/packet.h"
#include "server.h"

// Global flags
uint8_t debugFlag = 0;            // Can add conditional statements with this flag to print out extra info

// Global variables (for signal handler)
int listeningUDPSocketDescriptor;
char* message;                    // Message received on UDP socket

// Array of connected client data structures
struct connectedClient connectedClients[MAX_CONNECTED_CLIENTS];

extern struct ConnectionPacketDelimiters connectionPacketDelimiters;
extern struct StatusPacketDelimiters statusPacketDelimiters;

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
  struct sockaddr_in clientUDPAddress;                // Client's UDP info

  // Set up server sockaddr_in data structure
  memset(&serverAddress, 0, sizeof(serverAddress));   // 0 out
  serverAddress.sin_family = AF_INET;                 // IPV4
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);  
  serverAddress.sin_port = htons(PORT);               // Port
  
  checkCommandLineArguments(argc, argv, &debugFlag);  // Check if user passed any arguments

  listeningUDPSocketDescriptor = setupUdpSocket(serverAddress, 1);  // Setup the UDP socket
  
  message = calloc(1, INITIAL_MESSAGE_SIZE);          // Space for incoming messages
  
  // Whether or not data is available at the socket. If it is, what kind.
  int udpStatus;

  // Continously listen for new UDP packets and new TCP connections
  while (1) {
    udpStatus = checkUdpSocket(listeningUDPSocketDescriptor, &clientUDPAddress, message, debugFlag);  // Check the UDP socket
    switch (udpStatus) {
      case 0:                                         // Nothing

      break;

      case 1:                                         // Something
      
      char* tempMessage = calloc(1, 2000);
      strcpy(tempMessage, message);
      const char* middle = connectionPacketDelimiters.middle;
      char* packetBeginning = calloc(1, 20);

      while (strncmp(tempMessage, middle, 1) != 0) {                     // While not at the end of username
        strncat(packetBeginning, tempMessage, 1); 
        tempMessage++;
      }
      printf("packet beginning: %s\n", packetBeginning);

      if (strcmp(packetBeginning, connectionPacketDelimiters.beginning) == 0) {
        // Read connection packet
        struct ConnectionPacketFields connectionPacketFields; // Struct to store the sent data in
        uint8_t validPacket = readConnectionPacket(message, &connectionPacketFields);
        if (validPacket == -1) {                        // Check if the packet is valid
          printf("Invalid connection packet received\n");
          continue;
        }

        // Find an empty connection client and fill it out with the info from the connection packet
        int emptyConnectedClientIndex = findEmptyConnectedClient(debugFlag);                                                // Find an empty connected client
        strcpy(connectedClients[emptyConnectedClientIndex].username, connectionPacketFields.username);                      // username
        connectedClients[emptyConnectedClientIndex].socketUdpAddress.sin_addr.s_addr = clientUDPAddress.sin_addr.s_addr;    // UDP address
        connectedClients[emptyConnectedClientIndex].socketUdpAddress.sin_port = clientUDPAddress.sin_port;                  // UDP port
        strcpy(connectedClients[emptyConnectedClientIndex].availableResources, connectionPacketFields.availableResources);  // Available resources
        memset(&connectionPacketFields, 0, sizeof(connectionPacketFields));                                                 // Clear out struct used to store sent data
        printAllConnectedClients();
        break;
      }
      else if (strcmp(packetBeginning, statusPacketDelimiters.beginning) == 0) {
        
      }

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
  free(message);
  close(listeningUDPSocketDescriptor);
  printf("\n");
  exit(0);
}

/*
  * Name: findEmptyConnectedClient
  * Purpose: Loop through the connectedClients array until an empty spot is found. Looks for unset
  * UDP port.
  * Input: debugFlag
  * Output:
  * - -1: All spots in the connected client array are full
  * - Anything else: Index of the empty spot in the array
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
  unsigned long udpAddress;                                                   // Not in human readable format
  unsigned short udpPort;
  char* username = calloc(1, USERNAME_SIZE);                                  // Username
  char* availableResources = calloc(1, RESOURCE_ARRAY_SIZE);

  for (i = 0; i < MAX_CONNECTED_CLIENTS; i++) {
    udpAddress = ntohl(connectedClients[i].socketUdpAddress.sin_addr.s_addr); // UDP address
    udpPort = ntohs(connectedClients[i].socketUdpAddress.sin_port);           // UDP port
    if (udpAddress == 0 && udpPort == 0) {                                    // Check if the client is empty
      i++;
      continue;
    }
    strcpy(username, connectedClients[i].username);
    strcpy(availableResources, connectedClients[i].availableResources);
    printf("CONNECTED CLIENT %d\n", i);
    printf("USERNAME: %s\n", username);                                       // Print username
    memset(username, 0, USERNAME_SIZE);                                       // 0 out for next iteration
    printf("UDP ADDRESS: %ld\n", udpAddress);                                 // Print UDP address
    printf("UDP PORT: %d\n", udpPort);                                        // Print UDP port
    printf("AVAILABLE RESOURCES: %s\n", availableResources);                  // Print available resources
    memset(availableResources, 0, RESOURCE_ARRAY_SIZE);                       // 0 out for next iteration
  }
  free(username);
  free(availableResources);
}
