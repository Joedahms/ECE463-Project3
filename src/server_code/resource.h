#ifndef RESOURCE_H
#define RESOURCE_H

#include "../common/network_node.h"
#include "../common/packet.h"

// A single available resource that a connected client possesses. Is a single link
// in a linked list containing all available resources.
struct Resource {
  char username[MAX_USERNAME];
  char filename[FILE_NAME_SIZE];
  struct Resource* next;
};

struct Resource* addResource(struct Resource*, char*, char*);
void printAllResources(struct Resource*);
char* makeResourceString(char*, struct Resource*, char*);

#endif
