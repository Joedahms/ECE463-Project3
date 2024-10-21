#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>

int sendConnectionPacket(struct sockaddr_in, bool);
void shutdownClient(int);
void getUserInput(char*);
void receiveMessageFromServer();
int getAvailableResources(char*, const char*);

#endif
