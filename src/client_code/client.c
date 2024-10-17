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

#include "../common/network_node.h"
#include "../common/packet.h"
#include "client.h"

// Global flags
uint8_t debugFlag = 0;  // Can add conditional statements with this flag to print out extra info

// Global variables (for signal handler)
int udpSocketDescriptor;
char* userInput;
char* buffer;

// Connection packet delimiters that are constant for all connection packets
// See packet.h & packet.c
extern struct ConnectionPacketDelimiters connectionPacketDelimiters;
extern struct StatusPacketDelimiters statusPacketDelimiters;

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
  struct sockaddr_in udpAddress;                                                    // Will leave empty because do not need to bind
  memset(&udpAddress, 0, sizeof(udpAddress));                                       // 0 out
  udpSocketDescriptor = setupUdpSocket(udpAddress, 0);                              // Setup udp socket without binding

  struct ConnectionPacketFields connectionPacketFields;                             // Struct to store the fields to send
  
  // Get available resources on client and add to connection packet
  char* availableResources = calloc(1, RESOURCE_ARRAY_SIZE);                        // Allocate space for the string of available resources
  if (getAvailableResources(availableResources, "Public") == -1) {
    exit(1);
  }
  strcpy(connectionPacketFields.availableResources, availableResources);            // Add the resource string to the connection packet
  free(availableResources);                                                         // Free available resources string

  // Get username
  strcpy(connectionPacketFields.username, getenv("USER"));                          // Set the username by using the USER environment variable

  // Build and send the connection packet
  char* connectionPacket = calloc(1, CONNECTION_PACKET_SIZE);                       // Allocate connection packet string
  buildConnectionPacket(connectionPacket, connectionPacketFields, debugFlag);       // Build the entire connection packet
  sendUdpMessage(udpSocketDescriptor, serverAddress, connectionPacket, debugFlag);  // Send connection packet to the server
  free(connectionPacket);                                                           // Free connection packet


  struct StatusPacketFields statusPacketFields;
  strcpy(statusPacketFields.status, "testing");
  char* statusPacket = calloc(1, STATUS_PACKET_SIZE);
  buildStatusPacket(statusPacket, statusPacketFields, debugFlag);       // Build the entire connection packet
  sendUdpMessage(udpSocketDescriptor, serverAddress, statusPacket, debugFlag);  // Send connection packet to the server
  free(statusPacket);                                                           // Free connection packet


  fd_set read_fds;

  buffer = calloc(1, 1000);

  // Constantly check user input
  while(1) {
    // Use select to handle user input and server messages simultaneously
    FD_ZERO(&read_fds);
    FD_SET(0, &read_fds);  // 0 is stdin (for user input)
    FD_SET(udpSocketDescriptor, &read_fds);  // The socket for receiving server messages

    int activity = select(udpSocketDescriptor + 1, &read_fds, NULL, NULL, NULL);

    if (activity < 0 && errno != EINTR) {
        perror("select error");
    }

    if (FD_ISSET(0, &read_fds)) {
      // Get user input and store in userInput buffer
      getUserInput(userInput);

      // User just pressed return
      if (strlen(userInput) == 0) {  
        continue;
      }
    }
    if (FD_ISSET(udpSocketDescriptor, &read_fds)) {
      int bytesReceived = recvfrom(udpSocketDescriptor, buffer, USER_INPUT_BUFFER_LENGTH, 0, NULL, NULL);
      printf("buffer: %s\n", buffer);
      int packetType = getPacketType(buffer);
      printf("packet type: %d\n", packetType);

      struct StatusPacketFields statusPacketFields;
      strcpy(statusPacketFields.status, "testing");
      char* statusPacket = calloc(1, STATUS_PACKET_SIZE);
      buildStatusPacket(statusPacket, statusPacketFields, debugFlag);       // Build the entire connection packet
      sendUdpMessage(udpSocketDescriptor, serverAddress, statusPacket, debugFlag);  // Send connection packet to the server
      free(statusPacket);                                                           // Free connection packet
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

// Get available resources on client and add to connection packet
/*
  * Name: getAvailableResources
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
  DIR* directoryStream = opendir(directoryName);                      // Open resource directory
  if (directoryStream == NULL) {                                      // Error opening the directory
    return -1;
    perror("Error opening resource directory");
  }
  struct dirent* directoryEntry;                                      // dirent structure for the entries in the resource directory
  while((directoryEntry = readdir(directoryStream)) != NULL) {        // Loop through the entire resource directory
    const char* entryName = directoryEntry->d_name;                   // Get the name of the entry
    if (strcmp(entryName, ".") == 0) {                                // Ignore current directory
      continue;
    }
    if (strcmp(entryName, "..") == 0) {                               // Ignore parent directory
      continue;
    }
    strcat(availableResources, directoryEntry->d_name);               // Add the entry name to the available resources
    strcat(availableResources, connectionPacketDelimiters.resource);  // Add the resource delimiter to show end of resource
  } 
  return 0;
}

