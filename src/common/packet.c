#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>

#include "packet.h"

// Strings designating certain points in the packet
struct PacketDelimiters packetDelimiters = {
  1,
  "$",        // middle
  9,
  "endpacket" // end
};

/*
  * Purpose: Get the type of a packet based on the first field of the packet
  * Input:
  * - The packet to get the type of
  * Output: Integer representing the type of the packet
  * -1 = invalid
  * 0 = connection
  * 1 = status
  * 2 = resource
  * Notes: Might look at using an enum for packet type
*/
int getPacketType(const char* packet) {
  char* packetCopy = calloc(1, MAX_PACKET);
  char* packetCopyStart = packetCopy;
  strcpy(packetCopy, packet);

  char* packetType = calloc(1, MAX_PACKET_TYPE);

  while (strncmp(packetCopy, packetDelimiters.middle, 1) != 0) {
    strncat(packetType, packetCopy, 1);
    packetCopy++;
  }

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
  // Type and a middle
  strcpy(builtPacket, packetFields.type);
  strncat(builtPacket, packetDelimiters.middle, packetDelimiters.middleLength);
  if (debugFlag) {
    printf("Packet after adding beginning and a middle: %s\n", builtPacket);
  }

  // Data
  strcat(builtPacket, packetFields.data);
  if (debugFlag) {
    printf("Packet after adding data and a middle: %s\n", builtPacket);
  }

  // End
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

/*
  * Purpose: Check if at the end of the data field of a packet
  * Input: 
  * - The packet
  * Output:
  * 0: Not at the end of data
  * 1: At the end of data
*/
bool checkEnd(char* packet) {
  if (strncmp(packet, packetDelimiters.end, packetDelimiters.endLength) == 0) {
    return true;
  }
  return false;
}

/*
  * Purpose: Read a single field from a packet. Reads the packet string until the field
  * delimiter is hit.
  * Input: 
  * - The packet to read the field from
  * - String to return the field in
  * - Delimiter marking the end of the field
  * - Debug flag
  * Output: None
*/
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
  * Purpose: Skip a field in a packet. Advances the packet pointer past the current field
  * Input: 
  * - Packet to skip a field in
  * - Delimiter marking the end of the field to skip
  * - Debug flag
  * Output: Packet with one less field
*/
char* skipPacketField(char* packet, const char* fieldDelimiter, bool debugFlag) {
  char* dummyField = calloc(1, 50);
  readPacketField(packet, dummyField, fieldDelimiter, debugFlag);
  packet += strlen(dummyField) + 1;
  free(dummyField);
  return packet;
}
