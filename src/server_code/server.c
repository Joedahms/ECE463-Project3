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
#include <pthread.h>

#include "../common/network_node.h"
#include "../common/packet.h"
#include "server.h"

uint8_t debugFlag = 0;            // Can add conditional statements with this flag to print out extra info

// Global variables (for signal handler)
int listeningUDPSocketDescriptor;
char* packet;                    // packet received on UDP socket

// Array of connected client data structures
struct ConnectedClient connectedClients[MAX_CONNECTED_CLIENTS];

extern struct ConnectionPacketDelimiters connectionPacketDelimiters;
extern struct StatusPacketDelimiters statusPacketDelimiters;
extern struct ResourcePacketDelimiters resourcePacketDelimiters;

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
  struct sockaddr_in clientUDPAddress;

  // Set up server sockaddr_in data structure
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;                 // IPV4
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);  
  serverAddress.sin_port = htons(PORT);
  
  checkCommandLineArguments(argc, argv, &debugFlag);  // Check if user passed any arguments

  listeningUDPSocketDescriptor = setupUdpSocket(serverAddress, 1);  // Setup the UDP socket
  
  packet = calloc(1, INITIAL_MESSAGE_SIZE);          // Space for incoming packets
  
  int udpStatus;
  int packetType;

  pthread_t processId;
  pthread_create(&processId, NULL, checkClientStatus, NULL);

  // Continously listen for new UDP packets and new TCP connections
  while (1) {
    udpStatus = checkUdpSocket(listeningUDPSocketDescriptor, &clientUDPAddress, packet, debugFlag);  // Check the UDP socket

    if (udpStatus == 0) { // Nothing available on the UDP socket
      continue;
    }
    packetType = getPacketType(packet);
    
    switch(packetType) {
      case 0:                                                               // Connection packet
      if (debugFlag) {
        printf("Connection packet received\n");
      }
      int connectionPacketReturn = handleConnectionPacket(packet, clientUDPAddress, debugFlag);
      break;

      case 1:                                                               // Status packet
      if (debugFlag) {
        printf("Status packet received\n");
      }

      struct StatusPacketFields statusPacketFields;
      memset(&statusPacketFields, 0, sizeof(statusPacketFields));
      uint8_t validPacket = readStatusPacket(packet, &statusPacketFields);
      if (validPacket == -1) {
          break;
      }

      int handleStatusReturn = handleStatusPacket(clientUDPAddress);
      break;

      case 2:
      if (debugFlag) {
        printf("Resource packet received\n");
      }
      int resourceReturn = handleResourcePacket(clientUDPAddress);

      default:
    }
  } // while(1)
  return 0;
} // main

/*
  * Purpose: Check if clients are still connected to the server. Send every connected client
  * a packet asking if they are still connected. If they send a response within the set time frame,
  * they are considered to still be connected. If they do not send a packet back, they are considered
  * to be no longer connected. If a client is no longer connected, its information is erased from the user
  * directory (connectedClients).
  * Input: None
  * Output: None
  * Notes: This function is run in a thread spawned from the main process. It is alive for the entire
  * duration of the main process. It only exits when the server shuts down.
*/
void* checkClientStatus() {
  struct StatusPacketFields statusPacketFields;
  strcpy(statusPacketFields.status, "testing");
  char* statusPacket = calloc(1, MAX_STATUS_PACKET_SIZE);
  buildStatusPacket(statusPacket, statusPacketFields, debugFlag);

  struct ConnectedClient* client;
  struct sockaddr_in clientUdpAddress;

  int clientIndex;
  while(1) {
    for (clientIndex = 0; clientIndex < 100; clientIndex++) {
      client = &connectedClients[clientIndex];
      clientUdpAddress = client->socketUdpAddress;

      // Check that client is initialized and connected
      if (clientUdpAddress.sin_addr.s_addr == 0 && clientUdpAddress.sin_port == 0) {
        continue;
      }
      if (client->status == 0) {
        continue;
      }

      client->status = 0;           // Assume client is disconnected and will not respond
      client->requestedStatus = 1;  // Requested a response from the client
      sendUdpMessage(listeningUDPSocketDescriptor, clientUdpAddress, statusPacket, debugFlag);
      if (debugFlag) {
        printf("Status packet sent to client: %d\n", clientIndex);
      }
    }
    
    usleep(STATUS_SEND_INTERVAL); // Give clients a chance to send responses
    
    // If a response was requested and the client didn't send a response, remove them from the "user directory"
    for (clientIndex = 0; clientIndex < 100; clientIndex++) {
      client = &connectedClients[clientIndex];
      if (client->requestedStatus == 1 && client->status == 0) {
        if (debugFlag) {
          printf("Client %d disconnected\n", clientIndex);
        }
        memset(client, 0, sizeof(*client));
      }
    }
  }
  free(statusPacket);
}


/*
* Purpose: Gracefully shutdown the server when the user enters
* ctrl-c. Closes the sockets and frees addrinfo data structure
* Input: The signal raised
* Output: None
*/
void shutdownServer(int signal) {
  free(packet);
  close(listeningUDPSocketDescriptor);
  printf("\n");
  exit(0);
}


/*
  * Purpose: Loop through the connectedClients array until an empty spot is found. Looks for unset
  * UDP port.
  * Input: debugFlag
  * Output:
  * - -1: All spots in the connected client array are full
  * - Anything else: Index of the empty spot in the array
*/
int findEmptyConnectedClient(uint8_t debugFlag) {
  int connectedClientsIndex;
  for (connectedClientsIndex = 0; connectedClientsIndex < MAX_CONNECTED_CLIENTS; connectedClientsIndex++) {
    int port = ntohs(connectedClients[connectedClientsIndex].socketUdpAddress.sin_port);
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


/*
  * Purpose: When the server receives a connection packet, this function handles the data
  * in that packet. It finds an empty connected client and enters the packet sender's information
  * into that empty spot.
  * Input: 
  * - The connection packet that was sent
  * - The address of the client who sent the packet
  * Output:
  * -1: Packet received is not a valid connection packet
  * 0: Packet received is a valid connection packet and it was successfully handled
*/
int handleConnectionPacket(char* packet, struct sockaddr_in clientUDPAddress, uint8_t debugFlag) {
  struct ConnectionPacketFields connectionPacketFields;
  memset(&connectionPacketFields, 0, sizeof(connectionPacketFields));
  uint8_t validPacket = readConnectionPacket(packet, &connectionPacketFields);
  if (validPacket == -1) {
    printf("Invalid connection packet received\n");
    return -1;
  }

  // Find an empty connection client and fill it out with the info from the connection packet
  int emptyConnectedClientIndex = findEmptyConnectedClient(debugFlag);
  strcpy(connectedClients[emptyConnectedClientIndex].username, connectionPacketFields.username);                      // username
  connectedClients[emptyConnectedClientIndex].socketUdpAddress.sin_addr.s_addr = clientUDPAddress.sin_addr.s_addr;    // UDP address
  connectedClients[emptyConnectedClientIndex].socketUdpAddress.sin_port = clientUDPAddress.sin_port;                  // UDP port
  strcpy(connectedClients[emptyConnectedClientIndex].availableResources, connectionPacketFields.availableResources);  // Available resources

  if (debugFlag) {
    printAllConnectedClients();
  }
  return 0;
}


/*
  * Purpose: When the server receives a status packet, this function handles the data
  * in that packet. It loops through all the connected clients and finds the one who sent
  * the status packet. The status of the client who sent the packet is set to indicate
  * that the client is still connected.
  * Input: 
  * - The address of the client who sent the status packet
  * Output:
  * 0: Always
*/
int handleStatusPacket(struct sockaddr_in clientUdpAddress) {
  int clientIndex;
  struct ConnectedClient* currentClient;

  for (clientIndex = 0; clientIndex < MAX_CONNECTED_CLIENTS; clientIndex++) {                             // Loop through all clients
    currentClient = &connectedClients[clientIndex];

    unsigned long currentClientAddress = connectedClients[clientIndex].socketUdpAddress.sin_addr.s_addr;  // Client's IP address
    unsigned short currentClientPort = connectedClients[clientIndex].socketUdpAddress.sin_port;           // Client's port

    unsigned long incomingAddress = clientUdpAddress.sin_addr.s_addr;                                     // IP address of packet sender
    unsigned short incomingPort = clientUdpAddress.sin_port;                                              // Port of packet sender

    if (currentClientAddress == 0 && currentClientPort == 0) {                                            // Empty client
      continue;
    }

    if (currentClientAddress == incomingAddress && currentClientPort == incomingPort) {                   // Client matches packet sender
      currentClient->status = 1;                                                                          // Packet sender is client. They sent a response and are still connected
    }
  }
  return 0;
}

int handleResourcePacket(struct sockaddr_in clientUdpAddress) {
  int clientIndex = 0;
  struct ConnectedClient* currentClient;

  for (clientIndex = 0; clientIndex < MAX_CONNECTED_CLIENTS; clientIndex++) {
    currentClient = &connectedClients[clientIndex];
  }

  return 0;
}
