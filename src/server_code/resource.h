#ifndef RESOURCE_H
#define RESOURCE_H

#include "../common/network_node.h"

struct Resource {
  char username[MAX_USERNAME];
  char filename[FILE_NAME_SIZE];
  struct Resource* next;
};

struct Resource* addResource(struct Resource*, char*, char*);

#endif
