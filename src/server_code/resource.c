#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resource.h"

/*
  * Purpose: Add a resource to the resource directory linked list. The new resource is added at the beginning of the list.
  * The head resource is the resource that was just added.
  * Input: 
  * - Head resource in the linked list
  * - Username of the new resource
  * - filename of the new resource
  * Output: The new head node in the resource directory
*/
struct Resource* addResource(struct Resource* headResource, char* username, char* filename) {
  struct Resource* currentResource = malloc(sizeof(struct Resource));

  strcpy(currentResource->username, username);
  strcpy(currentResource->filename, filename);
  currentResource->next = headResource;
  headResource = currentResource;
  return headResource;
}

/*
  * Purpose: Print out all available resources. Print all the fields of the resource type. Traverses
  * the linked list that the available resources are stored in.
  * Input:
  * - None
  * Output: None
*/
void printAllResources(struct Resource* headResource) {
  printf("\n*** PRINTING ALL RESOURCES***\n");
  struct Resource* currentResource = headResource;
  while (currentResource) {
    printf("USERNAME: %s\n", currentResource->username);
    printf("FILENAME: %s\n", currentResource->filename);
    currentResource = currentResource->next;
  }
  printf("\n");
}

char* makeResourceString(char* resourceString, struct Resource* headResource, char* delimiter) {
  struct Resource* currentResource = headResource;
  while (currentResource) {
    strncat(resourceString, currentResource->username, strlen(currentResource->username));
    strncat(resourceString, delimiter, strlen(delimiter));
    strncat(resourceString, currentResource->filename, strlen(currentResource->filename));
    strncat(resourceString, delimiter, strlen(delimiter));
    currentResource = currentResource->next;
  }
  return resourceString;
}
