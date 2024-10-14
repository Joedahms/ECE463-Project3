#ifndef CLIENT_H
#define CLIENT_H

void shutdownClient(int);                                             // Gracefully shutdown client
void getUserInput(char*);                                             // Read user input from stdin
void receiveMessageFromServer();                                      // Receive a message from the server

#endif
