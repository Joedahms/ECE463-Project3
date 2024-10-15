#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>

#include "packet.h"

struct ConnectionPacket connectionPacket = {
  "startconnect",
  "$",
  "endconnect"
};

void buildConnectionPacket(char* builtPacket, struct sockaddr_in packetSource) {
  strncat(builtPacket, connectionPacket.beginning, strlen(connectionPacket.beginning)); // Add beginning field to packet
  char username[20] = "username";                                                       // Hard coded username for now
  strncat(builtPacket, connectionPacket.end, strlen(connectionPacket.end));             // Add end field to packet

}
