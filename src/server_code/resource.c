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

/*
  * Purpose: When a user disconnects, this function removes their resources from the resource directory.
  * It loops through the resouce directory looking for resources with a username matching the disconnected user.
  * It then removes those links.
  * Input: 
  * - Username of the disconnected user
  * - The resource at the head of the resource directory
  * - Debug flag
  * Output:
  * - Resource at the head of the resource directory. If resources are removed from the beginning of the 
  * - directory, the head will change.
*/
struct Resource* removeUserResources(char* username, struct Resource* headResource, bool debugFlag) {
  struct Resource* previousResource;
  struct Resource* currentResource = headResource;

  bool atHead = true;
  bool atEnd = false;

  char* resourceName = calloc(1, 20);
  while (!atEnd) {
    if (strcmp(currentResource->username, username) == 0) { // Link has username of disconnected user
      if (atHead) {
        headResource = removeHeadResource(headResource, debugFlag);
      }
      else {
        currentResource = removeNonHeadResource(previousResource, currentResource, &atEnd, debugFlag);
        if (atEnd) {
          break;
        }
      }
    }
    else {
      atHead = false;
    }
    previousResource = currentResource;
    currentResource = currentResource->next;
    if (!currentResource) {
      atEnd = true;
    }
  }
  if (debugFlag) {
    printf("Resource directory after removing user %s resources", username);
    printAllResources(headResource);
  }
  return headResource;
}

/*
  * Purpose: Print out username and filename of a resource and the one after it in the
  * resource directory.
  * Input: 
  * - Resource to print username and filename
  * - Name of the resource for printing purposes
  * Output: None
*/
void printResourceInfo(struct Resource* resource, char* resourceName) {
  printf("%s->username: %s\n", resourceName, resource->username);
  printf("%s->filename: %s\n", resourceName, resource->filename);
  printf("%s->next->username: %s\n", resourceName, resource->next->username);
  printf("%s->next->filename: %s\n", resourceName, resource->next->filename);
}

/*
  * Purpose: Remove the resource at the head of the resource directory
  * Input: 
  * - The resource at the head of the resource directory
  * - debugFlag
  * Output: New head of resource directory
*/
struct Resource* removeHeadResource(struct Resource* headResource, bool debugFlag) {
  if (debugFlag) {
    printf("Removing resource %s at head\n", headResource->filename);
    char* resourceName = calloc(1, 20);
    strcpy(resourceName, "headResource");
    printResourceInfo(headResource, resourceName);
    free(resourceName);
  }
  headResource = headResource->next;
  return headResource;
}

/*
  * Purpose: Remove a non head resource from the resource directory.
  * Input: 
  * - The resource before the one to remove
  * - The resource to remove
  * - Whether or not the resource removed was the last resource
  * - Debug flag
  * Output: Current resource after removing the resource. Before removal if R1 = Previous Resource, R2 = Current Resource, 
  * R3 = Next Resource, R3 would be returned, as R2 would have been removed.
*/
struct Resource* removeNonHeadResource(struct Resource* previousResource, struct Resource* currentResource, bool* atEnd, bool debugFlag) {
  if (debugFlag) {
    char* resourceName = calloc(1, 20);
    printf("BEFORE REMOVAL\n");
    printf("Removing resource %s not at head\n", currentResource->filename);
    strcpy(resourceName, "previousResource");
    printResourceInfo(previousResource, resourceName);
    strcpy(resourceName, "currentResource");
    printResourceInfo(previousResource, resourceName);
    free(resourceName);
  }

  // Indicate that at end of linked list if the last element is removed
  if (currentResource->next->next == NULL) {
    previousResource->next = NULL;
    *atEnd = true;
    return currentResource;
  }

  previousResource->next = currentResource->next; 
  currentResource = previousResource->next;

  if (debugFlag) {
    char* resourceName = calloc(1, 20);
    printf("AFTER REMOVAL\n");
    strcpy(resourceName, "previousResource");
    printResourceInfo(previousResource, resourceName);
    strcpy(resourceName, "currentResource");
    printResourceInfo(previousResource, resourceName);
    free(resourceName);
  }

  return currentResource;
}

