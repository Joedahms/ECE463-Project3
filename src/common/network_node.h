// Template for function comments
/*
  * Purpose:
  * Input: 
  * Output:
*/

#ifndef NETWORK_NODE_H
#define NETWORK_NODE_H

#define PORT 3941                     // Port server is listening on
#define FILE_NAME_SIZE 50             // Maximum file name length
#define MAX_FILE_SIZE 5000            // Maximum file length
#define INITIAL_MESSAGE_SIZE 200      // Max size of the initial calibration message
#define USER_INPUT_BUFFER_LENGTH 40   // Max size of message a user can input
#define MAX_USERNAME 20               // Max size of a username
#define MAX_RESOURCE_ARRAY 2000       // Max size of a string containing the available resources on a client
#define STATUS_SIZE 10

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdint.h>
#include <stdbool.h>

void checkCommandLineArguments(int, char**, bool*);
void sendUdpMessage(int, struct sockaddr_in, char*, bool);
void printReceivedMessage(struct sockaddr_in, int, char*, bool);

// File I/O
int readFile(char*, char*, bool);
int writeFile(char*, char*, size_t);

int setupUdpSocket(struct sockaddr_in, bool);
int checkUdpSocket(int, struct sockaddr_in*, char*, bool);
int handleErrorNonBlocking(int);

#endif
