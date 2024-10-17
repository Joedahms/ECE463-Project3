#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>

#include "packet.h"

// Delimiters in a connection packet. These fields are the same in every connection packet.
// To use this struct, extern it at the top of the file where it is used.
struct ConnectionPacketDelimiters connectionPacketDelimiters = {
  "startconnect", // beginning 
  "$",            // middle
  "%",            // resource
  "endconnect"    // end
};

struct StatusPacketDelimiters statusPacketDelimiters = {
  "startstatus",
  "$",
  "endstatus"
};

int getPacketType(const char* packet) {
  char* temppacketStart = calloc(1, CONNECTION_PACKET_SIZE);    // Allocate size for a copy of the incoming packet
  strcpy(temppacketStart, packet);                             // Copy the incoming packet
  char* temppacket = temppacketStart;
  char* middle = calloc(1, 20);
  strcpy(middle, connectionPacketDelimiters.middle);   // Set the middle delimiter to the middle delimiter of a connecion packet
                                                            // Probably need to make the middle delimiter common between all types of packets
  char* packetBeginning = calloc(1, 20);                    // Space for string for analyzing beginning of incoming packet

  while (strncmp(temppacket, middle, 1) != 0) {            // Traverse the beginning of the packet until a middle delimiter is hit
    strncat(packetBeginning, temppacket, 1);               // Add a character from the incoming packet to the packet beginning string
    temppacket++;                                          // Go to the next character
  }

  int returnVal = -1;
  if (strcmp(packetBeginning, connectionPacketDelimiters.beginning) == 0) {
    returnVal = 0;
  }
  else if (strcmp(packetBeginning, statusPacketDelimiters.beginning) == 0) {
    returnVal = 1;
  }

  free(temppacketStart);
  free(middle);
  free(packetBeginning);
  return returnVal;
}

/*
  * Name: buildConnectionPacket
  * Purpose: Using a passed in struct containing the fields of a connection packet, build a connection
  * packet in the form of a string.
  * Input: 
  * - String where the completed packet is to go
  * - struct containing the information the packet is to contain
  * - Debug flag
  * Output: None
*/
void buildConnectionPacket(char* builtPacket, struct ConnectionPacketFields connectionPacketFields, uint8_t debugFlag) {
  // Add beginning and middle delimiter
  strncat(builtPacket, connectionPacketDelimiters.beginning, strlen(connectionPacketDelimiters.beginning));
  strcat(builtPacket, connectionPacketDelimiters.middle);
  if (debugFlag) {
    printf("Connection packet after adding beginning and a middle: %s\n", builtPacket);
  }

  // Add username and a middle delimiter
  strcat(builtPacket, connectionPacketFields.username);                                           // Add username field to packet
  strcat(builtPacket, connectionPacketDelimiters.middle);
  if (debugFlag) {
    printf("Connection packet after adding username and a middle: %s\n", builtPacket);
  }

  // Add available resources and middle delimiter
  strcat(builtPacket, connectionPacketFields.availableResources);
  strcat(builtPacket, connectionPacketDelimiters.middle);
  if (debugFlag) {
    printf("Connection packet after adding availableResources and a middle: %s\n", builtPacket);
  }

  // Add end delimiter and if the debug flag is set, print out the whole packet
  strcat(builtPacket, connectionPacketDelimiters.end);
  if (debugFlag) {
    printf("Entire connection packet: %s\n", builtPacket);
  }
}

/*
  * Name: readConnectionPacket
  * Purpose: Read a connection packet that has been sent
  * Input:
  * - String containing the packet that was sent
  * - Struct to put the read out fields into
  * Output:
  * - -1: Packet invalid. Error could be on client's end or in transmission.
  * - 0: Valid packet.
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

void buildStatusPacket(char* builtPacket, struct StatusPacketFields statusPacketFields, uint8_t debugFlag) {
  // Add beginning and middle delimiter
  strncat(builtPacket, statusPacketDelimiters.beginning, strlen(statusPacketDelimiters.beginning));
  strcat(builtPacket, statusPacketDelimiters.middle);
  if (debugFlag) {
    printf("Status packet after adding beginning and a middle: %s\n", builtPacket);
  }

  // Add status and a middle delimiter
  strcat(builtPacket, statusPacketFields.status);
  strcat(builtPacket, statusPacketDelimiters.middle);
  if (debugFlag) {
    printf("Status packet after adding status and a middle: %s\n", builtPacket);
  }

  // Add end delimiter and if the debug flag is set, print out the whole packet
  strcat(builtPacket, statusPacketDelimiters.end);
  if (debugFlag) {
    printf("Entire status packet: %s\n", builtPacket);
  }
}

int readStatusPacket(char* packetToBeRead, struct StatusPacketFields* statusPacketFields) {
  // Delimiters
  const char* beginning = statusPacketDelimiters.beginning;
  const char* middle = statusPacketDelimiters.middle;
  const char* end = statusPacketDelimiters.end;

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
  while (strncmp(packetToBeRead, middle, 1) != 0) {                     // While not at the end of status
    strncat(statusPacketFields->status, packetToBeRead, 1); 
    packetToBeRead++;
  }
  packetToBeRead++; // Past middle delimiter

  return 0;
}

