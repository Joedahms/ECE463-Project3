#ifndef PACKET_H
#define PACKET_H

#define CONNECTION_PACKET_SIZE 200

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

struct ConnectionPacket {
  char beginning[20];
  char delimiter[20];
  char end[20];
};

void buildConnectionPacket(char*, struct sockaddr_in);

#endif
