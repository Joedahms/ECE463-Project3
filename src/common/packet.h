#ifndef PACKET_H
#define PACKET_H

#define CONNECTION_PACKET_SIZE 200
#define STATUS_PACKET_SIZE 200

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include "network_node.h"

// Delimiters in a connection packet. The same in every connection packet
struct ConnectionPacketDelimiters {
  char beginning[20]; // Beginning of the packet
  char middle[20];    // Seperate the fields of the packet
  char resource[20];  // Seperate the resources
  char end[20];       // End of the packet
};

// Fields in a connection packet that will be set by the sender and read by the receiver
struct ConnectionPacketFields {
  char username[USERNAME_SIZE];                 // Username of the client
  char availableResources[RESOURCE_ARRAY_SIZE]; // Filenames of files that the client is willing to share
};

struct StatusPacketDelimiters {
  char beginning[20]; 
  char middle[20];
  char end[20];
};

struct StatusPacketFields {
  char status[STATUS_SIZE];
};

struct ResourcePacketDelimiters {
  char beginning[20]; 
  char middle[20];
  char end[20];
};

struct ResourcePacketFields {
  char test[20];
};

int getPacketType(const char*);

void buildConnectionPacket(char*, struct ConnectionPacketFields, uint8_t);
int readConnectionPacket(char*, struct ConnectionPacketFields*);

void buildStatusPacket(char*, struct StatusPacketFields, uint8_t);
int readStatusPacket(char*, struct StatusPacketFields*);

void buildResourcePacket(char*, struct ResourcePacketFields, uint8_t);
int readResourcePacket(char*, struct ResourcePacketFields*);

#endif
