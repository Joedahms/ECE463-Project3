#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>

#include "packet.h"

/*
// Strings designating certain points in the packet
struct PacketDelimiters packetDelimiters = {
  1,
  "$",        // middle
  9,
  "endpacket" // end
};
*/

struct PacketDelimiters packetDelimiters = {
  1,
  "$",          // field
  1,
  "&",          // subfield
  9,
  "endpacket"   // end
}

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
int getPacketType(const char* packet, bool debugFlag) {
  int returnVal = -1;
  int i = 0;
  for (i = 0; i < NUM_PACKET_TYPES; i++) {
    if (strcmp(packetType, packetTypes[i]) == 0) {
      returnVal = i;
    }
  }

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
  // Type
  strcpy(builtPacket, packetFields.type);
  strncat(builtPacket, packetDelimiters.field, packetDelimiters.fieldLength);
  if (debugFlag) {
    printf("Packet after adding type: %s\n", builtPacket);
  }

  // Data
  strcat(builtPacket, packetFields.data);
  strncat(builtPacket, packetDelimiters.field, packetDelimiters.fieldLength);
  if (debugFlag) {
    printf("Packet after adding data: %s\n", builtPacket);
  }

  // End
  strncat(builtPacket, packetDelimiters.end, packetDelimiters.endLength);
  if (debugFlag) {
    printf("Entire packet: %s\n", builtPacket);
  }
}

/*
  * Purpose: Read a packet that has been sent by another network node. Take the packet in as a string and extract
  * its fields into the the the packet fields data type
  * Input:
  * - String containing the packet that was sent
  * - Struct to put the read out fields into
  * Output:
  * - 0
*/
int readPacket(char* packetToBeRead, struct PacketFields* packetFields, bool debugFlag) {
  char* packet = calloc(1, MAX_PACKET);
  char* packetStart = packet;
  strcpy(packet, packetToBeRead);

  // Type
  readPacketField(packet, packetFields->type, debugFlag);

  // Data
  while (checkEnd(packet) == false) {
    readPacketField(packet, packetFields->data, debugFlag);
  }

  free(packetStart);
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
static bool checkEnd(char* packet) {
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
  * - Debug flag
  * Output: The packet after reading the field from it
*/
static char* readPacketField(char* packet, char* field, bool debugFlag) {
  if (debugFlag) {
    printf("Reading field from packet: %s\n", packet);
  }

  while(strncmp(packet, packetDelimiters.field, packetDelimiters.fieldLength) != 0) {
    strncat(field, packet, 1);
    packet++;
  }
  packet += packetDelimiters.fieldLength;

  if (debugFlag) {
    printf("Field read: %s\n", field);
  }
  return packet;
}

void readPacketSubfield(char* packetField, char* packetSubfield, bool debugFlag) {
  if (debugFlag) {
    printf("Reading subfield from packet field: %s\n", packetField);
  }

  while(strncmp(packetField, packetDelimiters.subfield, packetDelimiters.subfieldLength) != 0) {
    strncat(packetSubfield, packetField, 1);
    packetField++;
  }
  packetField += packetDelimiters.subfieldLength;

  if (debugFlag) {
    printf("Subfield read: %s\n", field);
  }
}
