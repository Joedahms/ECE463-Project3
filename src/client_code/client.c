#define FILE_NAME_SIZE 50

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>

#include "../common/network_node.h"
#include "../common/packet.h"
#include "client.h"

// Global flags
bool debugFlag = false;  // Can add conditional statements with this flag to print out extra info

// Global variables (for signal handler)
int udpSocketDescriptor;
char* userInput;
char* buffer;

// Packet delimiters that are constant for all packets
// See packet.h & packet.c
extern struct PacketDelimiters packetDelimiters;

// Main
int main(int argc, char* argv[]) {
  // Assign callback function for ctrl-c
  signal(SIGINT, shutdownClient);

  // Allocate memory for user input
  userInput = calloc(1, USER_INPUT_BUFFER_LENGTH);
  
  // Socket address data structure of the server
  struct sockaddr_in serverAddress;

  // Setup address of server to send to
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(PORT);
  serverAddress.sin_addr.s_addr = INADDR_ANY;

  // Check command line arguments
  checkCommandLineArguments(argc, argv, &debugFlag);
 
  // Setup UDP socket
  struct sockaddr_in udpAddress;
  memset(&udpAddress, 0, sizeof(udpAddress));
  udpSocketDescriptor = setupUdpSocket(udpAddress, 0);

  if (sendConnectionPacket(serverAddress, debugFlag) == -1) {
    printf("Error sending connection packet\n");
  }

  fd_set read_fds;
  buffer = calloc(1, 1000);

  while(1) {
    // Use select to handle user input and server messages simultaneously
    FD_ZERO(&read_fds);
    FD_SET(0, &read_fds); // 0 is stdin (for user input)
    FD_SET(udpSocketDescriptor, &read_fds);  // The socket for receiving server messages

    int activity = select(udpSocketDescriptor + 1, &read_fds, NULL, NULL, NULL);

    if (activity < 0 && errno != EINTR) {
        perror("select error");
    }

    if (FD_ISSET(0, &read_fds)) {
      getUserInput(userInput);

      if (strcmp(userInput, "test") == 0) {
        sendResourcePacket(serverAddress, debugFlag);
      }

      // User just pressed return
      if (strlen(userInput) == 0) {
        continue;
      }
    }
    if (FD_ISSET(udpSocketDescriptor, &read_fds)) { // Message in UDP socket queue
      int bytesReceived = recvfrom(udpSocketDescriptor, buffer, USER_INPUT_BUFFER_LENGTH, 0, NULL, NULL);

      int packetType = getPacketType(buffer);
      switch(packetType) {
        case 0: // Connection
        break;

        case 1: // Status
        struct PacketFields packetFields;
        memset(&packetFields, 0, sizeof(packetFields));
        strcpy(packetFields.type, "status");
        strcat(packetFields.data, "testing");
        strcat(packetFields.data, "$");
        char* statusPacket = calloc(1, MAX_PACKET);
        buildPacket(statusPacket, packetFields, debugFlag);
        sendUdpMessage(udpSocketDescriptor, serverAddress, statusPacket, debugFlag);
        free(statusPacket);

        break;

        case 2: // Resource
        printf("packet: %s\n", buffer);

        break;

        default:
      }

    }
  }
	return 0;
} 

/*
 * Name: shutdownClient
 * Purpose: Gracefully shutdown the client.
 * Input: Signal received
 * Output: None
 */
void shutdownClient(int signal) {
  free(userInput);            // Free user input
  free(buffer);
  close(udpSocketDescriptor); // Close UDP socket
  printf("\n");
  exit(0);
}

/*
  * Name: getUserInput
  * Purpose: Get user input from standard in and remove the newline
  * Input: Buffer to store user input in
  * Output: None
*/
void getUserInput(char* userInput) {
  fgets(userInput, USER_INPUT_BUFFER_LENGTH, stdin);  // Get the input
  userInput[strcspn(userInput, "\n")] = 0;            // Remove \n
}

/*
  * Name: receiveMessageFromServer
  * Purpose: Receive a UDP message from the server
  * Input: 
  * - None
  * Output:
  * - None
*/
void receiveMessageFromServer() {
    char buffer[USER_INPUT_BUFFER_LENGTH];
    memset(buffer, 0, sizeof(buffer));  // Clear the buffer before receiving a new message
    int bytesReceived = recvfrom(udpSocketDescriptor, buffer, USER_INPUT_BUFFER_LENGTH, 0, NULL, NULL);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';   // Null-terminate the received string
        printf("Message from server: %s\n", buffer);
    } else {
        perror("Error receiving message from server");
    }
}

/*
  * Purpose: Get the available resources on the client and add them to the available
  * resources string.
  * Input: 
  * - String to put the available resources in
  * - Name of the directory where the available resources are located
  * Output:
  * - -1: Error
  * - 0: Success
*/
int getAvailableResources(char* availableResources, const char* directoryName) {
  DIR* directoryStream = opendir(directoryName);
  if (directoryStream == NULL) {
    return -1;
    perror("Error opening resource directory");
  }
  struct dirent* directoryEntry;
  while((directoryEntry = readdir(directoryStream)) != NULL) {        // Loop through the entire resource directory
    const char* entryName = directoryEntry->d_name;
    if (strcmp(entryName, ".") == 0) {                                // Ignore current directory
      continue;
    }
    if (strcmp(entryName, "..") == 0) {                               // Ignore parent directory
      continue;
    }
    strcat(availableResources, directoryEntry->d_name);               // Add the entry name to the available resources
    strcat(availableResources, packetDelimiters.middle);
  } 
  return 0;
}

/*
  * Purpose: Send a connection packet to the specified server
  * Input: 
  * - Socket address structure of the server to send the connection packet to
  * - Debug flag
  * Output:
  * -1: Error constructing or sending the connection packet, the packet was not sent
  * 0: Packet successfully sent
*/
int sendConnectionPacket(struct sockaddr_in serverAddress, bool debugFlag) {
  struct PacketFields packetFields; 
  strcpy(packetFields.type, "connection");

  strcpy(packetFields.data, getenv("USER"));
  strncat(packetFields.data, packetDelimiters.middle, packetDelimiters.middleLength);

  char* availableResources = calloc(1, MAX_DATA);
  if (getAvailableResources(availableResources, "Public") == -1) {
    return -1;
  }
  strcat(packetFields.data, availableResources);
  free(availableResources);

  char* packet = calloc(1, MAX_PACKET);
  buildPacket(packet, packetFields, debugFlag);
  sendUdpMessage(udpSocketDescriptor, serverAddress, packet, debugFlag);
  free(packet);

  return 0;
}

void sendResourcePacket(struct sockaddr_in serverAddress, bool debugFlag) {
  struct PacketFields packetFields;
  strcpy(packetFields.type, "resource");
  char* packet = calloc(1, MAX_PACKET);
  buildPacket(packet, packetFields, debugFlag);
  sendUdpMessage(udpSocketDescriptor, serverAddress, packet, debugFlag);
  free(packet);
}
