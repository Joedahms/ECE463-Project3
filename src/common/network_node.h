// Template for function comments
/*
  * Purpose:
  * Input: 
  * Output:
*/

#ifndef NETWORK_NODE_H
#define NETWORK_NODE_H

#define PORT 3941                                                       // Port server is listening on
#define FILE_NAME_SIZE 50                                               // Maximum file name length in bytes
#define MAX_FILE_SIZE 5000                                              // Maximum file length in bytes
#define INITIAL_MESSAGE_SIZE 200                                        // Max size of the initial calibration message
#define USER_INPUT_BUFFER_LENGTH 40                                     // Max size of message a user can input
#define MAX_USERNAME 20                                                // Max size of a username
#define MAX_RESOURCE_ARRAY 2000                                        // Max size of a string containing the available resources on a client
#define STATUS_SIZE 10

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdint.h>
#include <stdbool.h>

void checkCommandLineArguments(int, char**, bool*);                  // Check how many command line arguments passed
void sendUdpMessage(int, struct sockaddr_in, char*, bool);           // Send a message over UDP
void printReceivedMessage(struct sockaddr_in, int, char*, bool);     // Print out message along with its source

// File I/O
int readFile(char*, char*, bool);                                    // Read from a file with read()
int writeFile(char*, char*, size_t);                                    // Write to a file with write()

int setupUdpSocket(struct sockaddr_in, bool);                        // Setup a UDP socket
int checkUdpSocket(int, struct sockaddr_in*, char*, bool);           // Check a UDP socket to see if it has any data in the queue
int handleErrorNonBlocking(int);                                        // Handle error when "reading" from non blocking socket

#endif
