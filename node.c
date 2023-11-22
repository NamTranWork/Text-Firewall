#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "node.h"

/* Finds the length of a string*/
static uint64_t string_len(char *word)
{
  uint64_t length = 0;
  while (word[length] != '\0')
  {
    length++;
  }
  return length;
}

/* Creates a Node object with the oldspeak and newspeak pair.*/
Node *node_create(char *oldspeak, char *newspeak){
  Node *n = (Node *) malloc(sizeof(Node));
  if (n) {
    n->prev = NULL;
    n->next = NULL;
    
    if (oldspeak == NULL)
    {
      n->oldspeak = NULL;
    }
    else
    {
      n->oldspeak = (char *) calloc(string_len(oldspeak) + 1, sizeof(char));
    
      for (uint64_t i = 0; i < string_len(oldspeak); i++)
      {
        n->oldspeak[i] = oldspeak[i];
      }
    }
    
    if (newspeak == NULL)
    {
      n->newspeak = NULL;
    }
    else
    {
      n->newspeak = (char *) calloc(string_len(newspeak) + 1, sizeof(char));
    
      for (uint64_t i = 0; i < string_len(newspeak); i++)
      {
        n->newspeak[i] = newspeak[i];
      }
    }
  }
  return n;
}

/* Frees the memory in the node's oldspeak, newspeak, and itself. */
void node_delete(Node **n) {
  if (*n) {
    free((*n)->oldspeak);
    free((*n)->newspeak);
    free(*n);
    *n = NULL;
  }
}

/* Prints out oldspeak -> newspeak if the node's oldspeak and newspeak are
defined and prints only oldspeak if newspeak does not exists. */
void node_print(Node *n) {
  if (n) 
  {
    if (n->oldspeak != NULL && n->newspeak != NULL)
    {
      fprintf(stdout,"%s -> %s\n", n->oldspeak, n->newspeak);
    }
    else if (n->oldspeak != NULL)
    {
      fprintf(stdout,"%s\n", n->oldspeak);
    }
  }
}
