// Template for function comments
/*
  * Name:
  * Purpose:
  * Input: 
  * Output:
*/

#ifndef NETWORK_NODE_H
#define NETWORK_NODE_H

#define PORT 3941                                                       // Port server is listening on
#define FILE_NAME_SIZE 50                                               // Maximum file name length in bytes
#define MAX_FILE_SIZE 5000                                              // Maximum file length in bytes
#define INITIAL_MESSAGE_SIZE 100                                        // Max size of the initial calibration message
#define MAX_CONNECTED_CLIENTS 100                                       // Maximum number of clients that can be connected to the server
#define USER_INPUT_BUFFER_LENGTH 40                                     // Max size of message a user can input
#define USERNAME_SIZE 20                                                // Max size of a username

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdint.h>

void checkCommandLineArguments(int, char**, uint8_t*);                  // Check how many command line arguments passed
void sendUdpMessage(int, struct sockaddr_in, char*, uint8_t);           // Send a message over UDP
void printReceivedMessage(struct sockaddr_in, int, char*, uint8_t);     // Print out message along with its source

// File I/O
int readFile(char*, char*, uint8_t);                                    // Read from a file with read()
int writeFile(char*, char*, size_t);                                    // Write to a file with write()

int setupUdpSocket(struct sockaddr_in, uint8_t);                        // Setup a UDP socket
int checkUdpSocket(int, struct sockaddr_in*, char*, uint8_t);           // Check a UDP socket to see if it has any data in the queue
int handleErrorNonBlocking(int);                                        // Handle error when "reading" from non blocking socket

#endif
