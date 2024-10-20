#ifndef CLIENT_H
#define CLIENT_H

void shutdownClient(int);
void getUserInput(char*);
void receiveMessageFromServer();
int getAvailableResources(char*, const char*);

#endif
