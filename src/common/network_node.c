#define MAX_PACKET_LENGTH 5000  // Upper limit on packet size (bytes)

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>

#include "network_node.h"

/*
  * Name: checkCommandLineArguments
  * Purpose: Check for command line arguments when starting up a network node.
  * At the moment it is only used for setting the debug flag.
  * Input: 
  * - Number of command line arguments
  * - The command line arguments
  * - Debug flag
  * Output: None
*/
void checkCommandLineArguments(int argc, char** argv, uint8_t* debugFlag) {
  char* programName = argv[0];
	switch (argc) { // Check how many command line arguments are passed
		case 1:
			printf("Running %s in normal mode\n", programName);
			break;
		case 2:
			if (strcmp(argv[1], "-d") == 0) { // Check if debug flag
				*debugFlag = 1;
				printf("Running %s in debug mode\n", programName);
			}
			else {
				printf("Invalid usage of %s", programName);
			}
			break;
    default:
			printf("Invalid usage of %s", programName);
	}
}

/*
  * Name: sendUdpMessage
  * Purpose: Send a message via UDP
  * Input: 
  * - Socket to send the message out on
  * - Socket address to send the message to
  * - The message to send
  * - Debug flag
  * Output: None
*/
void sendUdpMessage(int udpSocketDescriptor, struct sockaddr_in destinationAddress, char* message, uint8_t debugFlag) {
  if (debugFlag) {
    printf("Sending UDP message:\n");
    printf("%s\n", message);
  }
  else {
    printf("Sending UDP message...\n"); 
  }

  // Send message to destinationAddress over udpSocketDescriptor
  int sendtoReturnValue = 0;
  sendtoReturnValue = sendto(udpSocketDescriptor, message, strlen(message), 0, (struct sockaddr *)&destinationAddress, sizeof(destinationAddress));
  if (sendtoReturnValue == -1) {
    perror("UDP send error");
    exit(1);
  }
  else {
    printf("UDP message sent\n");
  }
}

/*
  * Name: printReceivedMessage
  * Purpose: Print out a message along with its source
  * Input: 
  * - Who sent the message
  * - How long the received message is
  * - The received message
  * - Debug flag
  * Output: None
*/
void printReceivedMessage(struct sockaddr_in sender, int bytesReceived, char* message, uint8_t debugFlag) {
  if (debugFlag) {
    unsigned long senderAddress = ntohl(sender.sin_addr.s_addr);
    unsigned short senderPort = ntohs(sender.sin_port);
    printf("Received %d byte message from %ld:%d:\n", bytesReceived, senderAddress, senderPort);
    printf("%s\n", message);
  }
  else {
    printf("Received %d byte message\n", bytesReceived);
  }
}

/*
  * Name: readFile
  * Purpose: Open a file and read from it
  * Input: 
  * - File name
  * - Buffer to store the read contents
  * - Debug flag
  * Output: 
  * - -1: Error
  * - 0: Success
*/
int readFile(char* fileName, char* buffer, uint8_t debugFlag) {
  // Open the file
  int fileDescriptor;
  printf("Opening file %s...\n", fileName);
  fileDescriptor = open(fileName, O_CREAT, O_RDWR); // Create if does not exist + read and write mode
  if (fileDescriptor == -1) {
    perror("Error opening file");
    return -1;
  }
  printf("File %s opened\n", fileName);

  // Get the size of the file in bytes
  struct stat fileInformation;
  if (stat(fileName, &fileInformation) == -1) {
    perror("Error getting file size");
    return -1;
  };
  unsigned long int fileSize = fileInformation.st_size;
  if (debugFlag) {
    printf("%s is %ld bytes\n", fileName, fileSize);
  }

  // Read out the contents of the file
  printf("Reading file...\n");
  ssize_t bytesReadFromFile = 0;
  bytesReadFromFile = read(fileDescriptor, buffer, fileSize);
  if (bytesReadFromFile == -1) {
    perror("Error reading file");
    return -1;
  }
  if (debugFlag) {
    printf("%zd bytes read from %s\n", bytesReadFromFile, fileName);
  }
  printf("File read successfully\n");
  return 0;
}

/*
  * Name: writeFile
  * Purpose: Open a file and write to it
  * Input: 
  * - Name of the file
  * - What to write to the file
  * - Length of data to be written to the file
  * Output:
  * - -1: Error
  * - 0: Success
*/
int writeFile(char* fileName, char* fileContents, size_t fileSize) {
  // Open file to write to
  int fileDesciptor;
  fileDesciptor = open(fileName, (O_CREAT | O_RDWR), S_IRWXU); // Create if doesn't exist. Read/write
  if (fileDesciptor == -1) {
    perror("Error opening file");
    return -1;
  }

  // Write to the new file
  int writeReturn = write(fileDesciptor, fileContents, fileSize);
  if (writeReturn == -1) {
    perror("File write error");
    return -1;
  }
  return 0;
}

/*
  * Name: checkUdpSocket
  * Purpose: Check if there is an incoming message on a UDP port. If there is then
  * return an integer depending on the type of message
  * Input:
  * - Address of the UDP port that is receiving messages.
  * - If message is received, socket address data structure to store the senders address in
  * - Buffer to read message into
  * - Debug flag
  * Output: 
  * - 0: No incoming packets
  * - 1: Information about the UDP/TCP info relationship on the clien
  * - 2: Plain text message
  * - 3: Put command
  * - 4: Get command
  * - 5: Invalid command
*/
int checkUdpSocket(int listeningUDPSocketDescriptor, struct sockaddr_in* incomingAddress, char* message, uint8_t debugFlag) {
  // Check for incoming messages
  socklen_t incomingAddressLength = sizeof(incomingAddress);
  int bytesReceived = recvfrom(listeningUDPSocketDescriptor, message, INITIAL_MESSAGE_SIZE, 0, (struct sockaddr *)incomingAddress, &incomingAddressLength);
  int nonBlockingReturn = handleErrorNonBlocking(bytesReceived);

  if (nonBlockingReturn == 1) {                 // No incoming messages
    return 0;                                   // Return 0
  }
  
  // Print out UDP message
  printReceivedMessage(*incomingAddress, bytesReceived, message, debugFlag); 
  return 0;
  
  // message
}

/*
  * Name: handleErrorNonBlocking
  * Purpose: Check the return after checking a non blocking socket
  * Input: The return value from checking the socket
  * Output:
  * - 0: There is data waiting to be read
  * - 1: No data waiting to be read
*/
int handleErrorNonBlocking(int returnValue) {
  if (returnValue == -1) {                          // Error
    if (errno == EAGAIN || errno == EWOULDBLOCK) {  // Errors occuring from no message on non blocking socket
      return 1;
    }
    else {                                          // Relevant error
      perror("Error when checking non blocking socket");
      exit(1);
      return 1;
    }
  }
  else {                                            // Got a message
    return 0;
  }
}
