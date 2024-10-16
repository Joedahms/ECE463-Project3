#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>

#include "packet.h"

// Delimiters in a connection packet. The same in every connection packet
struct ConnectionPacketDelimiters connectionPacketDelimiters = {
  "startconnect", // beginning 
  "$",            // middle
  "endconnect"    // end
};

/*
  * Name: buildConnectionPacket
  * Purpose: Construct a connection packet
  * Input: 
  * - String where the packet is
  * - Address structure of the source of the packet
  * - Debug flag
  * Output: None
*/
void buildConnectionPacket(char* builtPacket, struct ConnectionPacketFields connectionPacketFields, uint8_t debugFlag) {
  strncat(builtPacket, connectionPacketDelimiters.beginning, strlen(connectionPacketDelimiters.beginning)); // Add beginning delimiter to packet
  strncat(builtPacket, connectionPacketFields.username, strlen(connectionPacketFields.username));                                                         // Add username field to packet
  strncat(builtPacket, connectionPacketDelimiters.end, strlen(connectionPacketDelimiters.end));             // Add end delimiter to packet
  if (debugFlag) {                                                                                          // If debug flag
    printf("Connection packet: %s\n", builtPacket);                                                         // Print out the whole packet
  }
}

/*
  * Name: readConnectionPacket
  * Purpose: Read a connection packet that has been sent
  * Input:
  * - String containing the packet that was sent
  * - The fields read from the packet
  * Output: Whether or not the packet is valid
*/
int readConnectionPacket(char* packetToBeRead, struct ConnectionPacketFields* readPacketFields) {
  // Delimiters
  char* beginning = connectionPacketDelimiters.beginning;
  char* middle = connectionPacketDelimiters.middle;
  char* end = connectionPacketDelimiters.end;

  // Check if beginning of the packet is correct
  if (strncmp(packetToBeRead, beginning, strlen(beginning)) != 0) {
    return -1;
  }
  packetToBeRead += strlen(beginning);                                  // Advance past the beginning
  int usernameLength = strlen(packetToBeRead) - strlen(end);            // packet is now just username and end
  strncpy(readPacketFields->username, packetToBeRead, usernameLength);  // Read the username into the returned type
  return 0;
}
