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
  * - The first resource in the resource directory
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

/*
  * Purpose: Take all the available resources and put them into one string. Add the username then the
  * filename of each resource
  * Input: 
  * - String to put all the resources in
  * - The first resource in the resource directory
  * - Delimiter to put between resources
  * Output:
*/
char* makeResourceString(char* resourceString, struct Resource* headResource, char* delimiter) {
  struct Resource* currentResource = headResource;
  while (currentResource) {
    strncat(resourceString, currentResource->username, strlen(currentResource->username));
    strcat(resourceString, delimiter);
    strncat(resourceString, currentResource->filename, strlen(currentResource->filename));
    strcat(resourceString, delimiter);
    currentResource = currentResource->next;
  }
  return resourceString;
}

struct Resource* removeUserResources(char* username, struct Resource* headResource, bool debugFlag) {
  struct Resource* previousResource;
  struct Resource* currentResource = headResource;

  bool atHead = true;

  while (currentResource) { // While not at end of resource list
    if (strcmp(currentResource->username, username) == 0) {
      if (atHead) {
        if (debugFlag) {
          printf("Removing resource %s at head\n", currentResource->filename);
          printf("currentResource->username: %s\n", currentResource->username);
          printf("currentResource->filename: %s\n", currentResource->filename);
          printf("currentResource->next->username: %s\n", currentResource->next->username);
          printf("currentResource->next->filename: %s\n", currentResource->next->filename);
        }
        headResource = currentResource->next;
      }
      else {
        if (debugFlag) {
          printf("BEFORE REMOVAL\n");
          printf("Removing resource %s not at head\n", currentResource->filename);
          printf("previousResource->username: %s\n", previousResource->username);
          printf("previousResource->filename: %s\n", previousResource->filename);
          printf("previousResource->next->username: %s\n", previousResource->next->username);
          printf("previousResource->next->filename: %s\n", previousResource->next->filename);
          printf("currentResource->username: %s\n", currentResource->username);
          printf("currentResource->filename: %s\n", currentResource->filename);
          printf("currentResource->next->username: %s\n", currentResource->next->username);
          printf("currentResource->next->filename: %s\n", currentResource->next->filename);
        }
        if (currentResource->next->next == NULL) {
          previousResource->next = NULL;
          break;
        }
        previousResource->next = currentResource->next; 
        currentResource = previousResource->next;
        if (debugFlag) {
          printf("AFTER REMOVAL\n");
          printf("previousResource->username: %s\n", previousResource->username);
          printf("previousResource->filename: %s\n", previousResource->filename);
          printf("previousResource->next->username: %s\n", previousResource->next->username);
          printf("previousResource->next->filename: %s\n", previousResource->next->filename);
          printf("currentResource->username: %s\n", currentResource->username);
          printf("currentResource->filename: %s\n", currentResource->filename);
          printf("currentResource->next->username: %s\n", currentResource->next->username);
          printf("currentResource->next->filename: %s\n", currentResource->next->filename);
        }
      }
    }
    else {
      atHead = false;
    }
    previousResource = currentResource;
    currentResource = currentResource->next;
  }
  if (debugFlag) {
    printf("Resource directory after removing user %s resources", username);
    printAllResources(headResource);
  }
  return headResource;
}
