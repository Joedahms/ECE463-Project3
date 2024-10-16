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
  "%",
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
  strcat(builtPacket, connectionPacketDelimiters.beginning); // Add beginning delimiter to packet
  strcat(builtPacket, connectionPacketDelimiters.middle);
  strcat(builtPacket, connectionPacketFields.username);                                                         // Add username field to packet
  strcat(builtPacket, connectionPacketDelimiters.middle);
  strcat(builtPacket, connectionPacketFields.availableResources);
  strcat(builtPacket, connectionPacketDelimiters.middle);
  strcat(builtPacket, connectionPacketDelimiters.end);             // Add end delimiter to packet
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
  const char* beginning = connectionPacketDelimiters.beginning;
  const char* middle = connectionPacketDelimiters.middle;
  const char* resource = connectionPacketDelimiters.resource;
  const char* end = connectionPacketDelimiters.end;

  // Beginning
  if (strncmp(packetToBeRead, beginning, strlen(beginning)) != 0) {     // Check if the beginning of the packet is correct
    return -1;
  }
  packetToBeRead += strlen(beginning);                                  // Advance past the beginning
  if (strncmp(packetToBeRead, "$", 1) != 0) {                           // Make sure there is a middle delimiter after the beginning
    return -1;
  }
  packetToBeRead++;                                                     // Past middle delimiter

  // Username
  while (strncmp(packetToBeRead, middle, 1) != 0) {                     // While not at the end of username
    strncat(readPacketFields->username, packetToBeRead, 1); 
    packetToBeRead++;
  }
  packetToBeRead++; // Past middle delimiter

  // Available resources
  while (strncmp(packetToBeRead, middle, 1) != 0) {                     // While not at the end of available resources
    strncat(readPacketFields->availableResources, packetToBeRead, 1);
    packetToBeRead++;
  }
  packetToBeRead++; // Past middle delimiter

  return 0;
}
