#ifndef PACKET_H
#define PACKET_H

#define CONNECTION_PACKET_SIZE 200

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include "network_node.h"

// Delimiters in a connection packet. The same in every connection packet
struct ConnectionPacketDelimiters {
  char beginning[20]; // Beginning of the packet
  char middle[20];    // Seperate the fields of the packet
  char end[20];       // End of the packet
};

// Fields in a connection packet that will be set by the sender and read by the receiver
struct ConnectionPacketFields {
  char username[USERNAME_SIZE];
};

void buildConnectionPacket(char*, struct ConnectionPacketFields, uint8_t);  // Build a connection packet with the delimiters and the fields
int readConnectionPacket(char*, struct ConnectionPacketFields*);            // Get the fields out of a connection packet

#endif
