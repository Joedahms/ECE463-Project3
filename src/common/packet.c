#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>

#include "packet.h"

struct ConnectionPacket connectionPacket = {"startconnect", "$", "endconnect"};

void buildConnectionPacket(char* builtPacket, struct sockaddr_in packetSource) {
  strncat(builtPacket, connectionPacket.beginning, strlen(connectionPacket.beginning));
  unsigned long sourceAddressInt = ntohl(packetSource.sin_addr.s_addr);
  printf("%ld\n", sourceAddressInt);
  char* sourceAddressString = malloc(20);
  snprintf(sourceAddressString, 20, "%ld", sourceAddressInt);
  printf("%s\n", sourceAddressString);
}
