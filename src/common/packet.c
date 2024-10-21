#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>

#include "packet.h"

struct PacketDelimiters packetDelimiters = {
  1,
  "$",        // middle
  9,
  "endpacket" // end
};

int getPacketType(const char* packet) {
  char* packetCopy = calloc(1, MAX_PACKET);
  char* packetCopyStart = packetCopy;
  strcpy(packetCopy, packet);

  char* packetType = calloc(1, MAX_PACKET_TYPE);

  while (strncmp(packetCopy, packetDelimiters.middle, 1) != 0) {
    strncat(packetType, packetCopy, 1);
    packetCopy++;
  }

  // -1 = invalid
  // 0 = connection
  // 1 = status
  // 2 = resource
  int returnVal = -1;
  int i = 0;
  for (i = 0; i < NUM_PACKET_TYPES; i++) {
    if (strcmp(packetType, packetTypes[i]) == 0) {
      returnVal = i;
    }
  }

  free(packetCopyStart);
  free(packetType);
  return returnVal;
}

/*
  * Purpose: Using a passed in struct containing the fields of a packet, build a
  * packet in the form of a string.
  * Input: 
  * - String where the completed packet is to go
  * - struct containing the information the packet is to contain
  * - Debug flag
  * Output: None
*/
void buildPacket(char* builtPacket, struct PacketFields packetFields, bool debugFlag) {
  // Add type and middle delimiter
  strcpy(builtPacket, packetFields.type);
  strncat(builtPacket, packetDelimiters.middle, packetDelimiters.middleLength);
  if (debugFlag) {
    printf("Packet after adding beginning and a middle: %s\n", builtPacket);
  }

  strcat(builtPacket, packetFields.data);
  if (debugFlag) {
    printf("Packet after adding data and a middle: %s\n", builtPacket);
  }

  // Add end delimiter
  strncat(builtPacket, packetDelimiters.end, packetDelimiters.endLength);
  if (debugFlag) {
    printf("Entire packet: %s\n", builtPacket);
  }
}

/*
  * Purpose: Read a packet that has been sent by another network node
  * Input:
  * - String containing the packet that was sent
  * - Struct to put the read out fields into
  * Output:
  * - -1: Packet invalid. Error could be on client's end or in transmission.
  * - 0: Valid packet.
*/
int readPacket(char* packetToBeRead, struct PacketFields* packetFields) {
  // Delimiters
  const char* middle = packetDelimiters.middle;
  const char* end = packetDelimiters.end;

  // Type
  while (strncmp(packetToBeRead, middle, 1) != 0) {
    strncat(packetFields->type, packetToBeRead, 1);
    packetToBeRead++;
  }
  packetToBeRead++; // Past middle delimiter

  // Data
  const int endLength = strlen(packetDelimiters.end);
  bool atEnd = false;

  while (!atEnd) {
    while (strncmp(packetToBeRead, middle, 1) != 0) {
      strncat(packetFields->data, packetToBeRead, 1);
      packetToBeRead++;
    }
    packetToBeRead++;

    if (strncmp(packetToBeRead, end, endLength) == 0) {
      atEnd = true;
    }
  }

  return 0;
}


bool checkEnd(char* packet) {
  if (strncmp(packet, packetDelimiters.end, packetDelimiters.endLength) == 0) {
    return true;
  }
  return false;
}


void readPacketField(char* packet, char* field, const char* fieldDelimiter, bool debugFlag) {
  if (debugFlag) {
    printf("Reading field from packet: %s\n", packet);     
  }

  char* packetCopy = calloc(1, MAX_PACKET);
  char* packetCopyBeginning = packetCopy;
  strcpy(packetCopy, packet);

  while(strncmp(packetCopy, fieldDelimiter, 1) != 0) {
    strncat(field, packetCopy, 1);
    packetCopy++;
  }

  if (debugFlag) {
    printf("Field read: %s\n", field);
  }
  free(packetCopyBeginning);
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
/*
void buildConnectionPacket(char* builtPacket, struct ConnectionPacketFields connectionPacketFields, bool debugFlag) {
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
*/


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
/*
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
*/

/*
void buildStatusPacket(char* builtPacket, struct StatusPacketFields statusPacketFields, bool debugFlag) {
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
*/
/*
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

  // Status
  while (strncmp(packetToBeRead, middle, 1) != 0) {                     // While not at the end of status
    strncat(statusPacketFields->status, packetToBeRead, 1); 
    packetToBeRead++;
  }
  packetToBeRead++; // Past middle delimiter

  return 0;
}

void buildResourcePacket(char* builtPacket, struct ResourcePacketFields resourcePacketFields, bool debugFlag) {
  // Add beginning and middle delimiter
  strncat(builtPacket, resourcePacketDelimiters.beginning, strlen(resourcePacketDelimiters.beginning));
  strcat(builtPacket, resourcePacketDelimiters.middle);
  if (debugFlag) {
    printf("Resource packet after adding beginning and a middle: %s\n", builtPacket);
  }

  // Add status and a middle delimiter
  strcat(builtPacket, resourcePacketFields.test);
  strcat(builtPacket, resourcePacketDelimiters.middle);
  if (debugFlag) {
    printf("Resource packet after adding status and a middle: %s\n", builtPacket);
  }

  // Add end delimiter and if the debug flag is set, print out the whole packet
  strcat(builtPacket, resourcePacketDelimiters.end);
  if (debugFlag) {
    printf("Entire resource packet: %s\n", builtPacket);
  }
}

int readResourcePacket(char* packetToBeRead, struct ResourcePacketFields* resourcePacketFields) {
  // Delimiters
  const char* beginning = resourcePacketDelimiters.beginning;
  const char* middle = resourcePacketDelimiters.middle;
  const char* end = resourcePacketDelimiters.end;

  // Beginning
  if (strncmp(packetToBeRead, beginning, strlen(beginning)) != 0) {     // Check if the beginning of the packet is correct
    return -1;
  }
  packetToBeRead += strlen(beginning);                                  // Advance past the beginning
  if (strncmp(packetToBeRead, "$", 1) != 0) {                           // Make sure there is a middle delimiter after the beginning
    return -1;
  }
  packetToBeRead++;                                                     // Past middle delimiter

  // Test
  while (strncmp(packetToBeRead, middle, 1) != 0) {                     // While not at the end of status
    strncat(resourcePacketFields->test, packetToBeRead, 1); 
    packetToBeRead++;
  }
  packetToBeRead++; // Past middle delimiter

  return 0;
}
*/
