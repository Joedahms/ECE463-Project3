#ifndef PACKET_H
#define PACKET_H

/*
#define MAX_CONNECTION_PACKET_SIZE 200
#define MAX_STATUS_PACKET_SIZE 200
#define MAX_RESOURCE_PACKET_SIZE 200
*/

#define MAX_PACKET 220
#define MAX_PACKET_TYPE 20
#define NUM_PACKET_TYPES 3
#define MAX_DATA 200

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdbool.h>

#include "network_node.h"

static const char* packetTypes[NUM_PACKET_TYPES] = {"connection", "status", "resource"};

struct PacketDelimiters {
  int middleLength;
  char middle[1];
  int endLength;
  char end[9];
};

struct PacketFields {
  char type[MAX_PACKET_TYPE];
  char data[MAX_DATA];
};

int getPacketType(const char*);
void buildPacket(char*, struct PacketFields, bool);
int readPacket(char*, struct PacketFields*);

bool checkEnd(char*);
void readPacketField(char*, char*, const char*, bool);

/*
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
*/


/*
void buildConnectionPacket(char*, struct ConnectionPacketFields, bool);
int readConnectionPacket(char*, struct ConnectionPacketFields*);

void buildStatusPacket(char*, struct StatusPacketFields, bool);
int readStatusPacket(char*, struct StatusPacketFields*);

void buildResourcePacket(char*, struct ResourcePacketFields, bool);
int readResourcePacket(char*, struct ResourcePacketFields*);
*/

#endif
