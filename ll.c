#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "node.h"
#include "ll.h"

uint64_t seeks = 0; /* Amount of lookups performed */
uint64_t links = 0; /* The total number of links traversed during lookups */

/* Returns 0 if strings first_str and second_str are equal. Returns 1
if not. */
static uint64_t string_cmp(char * first_str, char * second_str)
{
  bool not_equal = false;
  uint64_t total_index = 0;
  
  while (first_str[total_index] != '\0' && second_str[total_index] != '\0')
  {
    if (first_str[total_index] != second_str[total_index])
    {
      not_equal = true;
      break;
    }
    total_index++;
  }
  
  if (not_equal == false && first_str[total_index] == '\0' &&
   second_str[total_index] == '\0')
  {
    return 0;
  }
  return 1;
}

/* The attributes of a LinkedList object */
struct LinkedList {
  uint32_t length;
  Node *head;
  Node *tail;
  bool mtf;
};

/* Creates a LinkedList object and its attributes. It also creates 
sentinel nodes with no oldspeak and newspeak words (NULL) and assigns
them to the head and tail of the LinkedList object. */
LinkedList *ll_create(bool mtf) {

  LinkedList *ll = (LinkedList *) malloc(sizeof(LinkedList));
 
  if (ll)
  {
    ll->mtf = mtf;
    ll->length = 0;
    ll->head = node_create(NULL, NULL);
    ll->tail = node_create(NULL, NULL);
    
    ll->head->prev = NULL;
    ll->head->next = ll->tail;
    ll->tail->prev = ll->head;
    ll->tail->next = NULL;
  }
  return ll;
}

/* Traverses through and free each node (including the sentinel nodes)*/
void ll_delete(LinkedList **ll) {
  while ((*ll)->head != NULL)
  {
    Node *next = NULL;
    next = (*ll)->head->next;
    node_delete(&((*ll)->head));
    (*ll)->head = next;
  }
  free(*ll);
  *ll = NULL;
}

/* Returns the length of the linkedlist */
uint32_t ll_length(LinkedList *ll) {
  return ll->length;
}

/* Checks to see if an oldspeak word is in a linkedlist. Returns NULL if
an oldspeak word is not in the linkedlist. Returns the node containing the 
oldspeak word if the word is present in the linkedlist. */
Node *ll_lookup(LinkedList *ll, char *oldspeak) {
  seeks++;
  for (Node *current = ll->head->next; current != ll->tail; current = current->next)
  {
    links++;
    if (string_cmp(current->oldspeak, oldspeak) == 0)
    {
      /* If the move-to-front option is activate, the node containing
      the oldspeak word is move to the front of the linkedlist. */
      if (ll->mtf == true)
      {
        current->prev->next = current->next;
        current->next->prev = current->prev;
        
        current->next = ll->head->next;
        current->prev = ll->head;
        ll->head->next->prev = current;
        ll->head->next = current;
      }
      return current;
    }
  }
  return NULL;
}

/* Inserts the oldspeak and its newspeak translation to the linkedlist. */
void ll_insert(LinkedList *ll, char *oldspeak, char *newspeak) {
  if(ll_lookup(ll, oldspeak) == NULL)
  {
    ll->length += 1;
    Node *new_node = node_create(oldspeak, newspeak);
    new_node->next = ll->head->next;
    new_node->prev = ll->head;
    ll->head->next->prev = new_node;
    ll->head->next = new_node;
  }
}

/* Prints all nodes in the linkedlist besides the sentinel nodes. */
void ll_print(LinkedList *ll) {
  for (Node *current = ll->head->next; current != ll->tail; current = current->next)
  {
    node_print(current);
  }
}

/* Set seeks to n_seeks and links to n_links */
void ll_stats(uint32_t *n_seeks, uint32_t *n_links) {
  *n_seeks = seeks;
  *n_links = links;
}
