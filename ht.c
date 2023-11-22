#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "node.h"
#include "ll.h"
#include "ht.h"
#include "city.h"

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

/* The attributes of a HashTable object */
struct HashTable {
  uint64_t salt;
  uint32_t size;
  uint32_t n_keys;
  uint32_t n_hits;
  uint32_t n_misses;
  uint32_t n_examined;
  bool mtf;
  LinkedList **lists;
};

/* Creates a HashTable object with size and mtf */
HashTable *ht_create(uint32_t size, bool mtf) {

  HashTable *ht = (HashTable *) malloc(sizeof(HashTable));
  
  if (ht != NULL)
  {
    ht->mtf = mtf;
    ht->salt = 0x9846e4f157fe8840;
    ht->n_hits = ht->n_misses = ht->n_examined = 0;
    ht->n_keys = 0;
    ht->size = size;
    ht->lists = (LinkedList **) calloc(size, sizeof(LinkedList *));
    
    if (!ht->lists)
    {
      free(ht);
      ht = NULL;
    }
  }
  
  return ht;
}

/* Deletes the HashTable object and its associated linked lists */
void ht_delete(HashTable **ht)
{
  if (*ht != NULL && ((*ht)->lists) != NULL)
  {
    for (uint64_t i = 0; i < (*ht)->size ; i++)
    {
      if ((*ht)->lists[i] != NULL)
      {
        ll_delete(&((*ht)->lists[i]));
      }
    }
    free((*ht)->lists);
    (*ht)->lists = NULL;
    
  }
  
  if (*ht != NULL)
  {
    free(*ht);
    *ht = NULL;
  }
}

/* Returns the size of the hashtable */
uint32_t ht_size(HashTable *ht) {
  return ht->size;
}

/* Looks through each linkedlist and returns the node that contains the
oldspeak word. Returns NULL if the word is not in the hash table. */
Node *ht_lookup(HashTable *ht, char *oldspeak){
  uint64_t index = hash(ht->salt, oldspeak) % ht->size;
  uint32_t sub_seek = 0;
  uint32_t sub_link = 0;
  uint32_t sub_link2 = 0;
  
  ll_stats(&sub_seek, &sub_link);
  
  Node * result = NULL;
  if (ht->lists[index] != NULL)
  {
    result = ll_lookup(ht->lists[index], oldspeak);
  }
  
  ll_stats(&sub_seek, &sub_link2);
  
  ht->n_examined += (sub_link2 - sub_link);
  
  if (result == NULL)
  {
    ht->n_misses += 1;
    return result;
  }
  ht->n_hits += 1;
  return result;
}

/* Inserts the oldspeak word and its newspeak translation to one of the
linked lists in the hashtable based on the index produced by the hash. */
void ht_insert(HashTable *ht, char *oldspeak, char *newspeak){
  uint64_t index = hash(ht->salt, oldspeak) % ht->size;
  
  if (ht->lists[index] == NULL)
  {
    LinkedList *ll = ll_create(ht->mtf);
    ht->lists[index] = ll;
    ll_insert(ht->lists[index], oldspeak, newspeak);
    ht->n_keys++;
  }
  else
  {
    bool insert = true;
    for (Node *current = ht->lists[index]->head->next; current != ht->lists[index]->tail; current = current->next)
    {
      if (string_cmp(current->oldspeak, oldspeak) == 0)
      {
        insert = false;
      }
    }
    
    if (insert == true)
    {
      ht->n_keys++;
    }
    ll_insert(ht->lists[index], oldspeak, newspeak);
  }
}

/* Returns the number of non-NULL linked lists in the hash table */
uint32_t ht_count(HashTable *ht) {
  
  uint32_t count = 0;
  
  for (uint32_t i = 0; i < ht_size(ht); i++)
  {
    if (ht->lists[i] != NULL)
    {
      count++;
    }
  }
  
  return count;
}

/* Prints all the nodes from the non-NULL linked lists in the hash table */
void ht_print(HashTable *ht) {
  for (uint64_t i = 0; i < ht_size(ht); i++)
  {
    if (ht->lists[i] != NULL)
    {
      ll_print(ht->lists[i]);
      printf("\n");
    }
  }
}

/* Sets each pointer to the value of the original variable of the relevant
statistic that the HashTable is keeping track off.*/
/* *nk tracks the number of keys present in the HashTable object *ht */
/* *nh tracks the number of hits from all the probes performed with the
HashTable object *ht */
/* *nm tracks the number of misses from all the probes performed with the
HashTable object *ht */
/* *ne tracks the number of linked examined from all the probes performed
with the HashTable object *ht */
void ht_stats(HashTable *ht, uint32_t *nk, uint32_t *nh, uint32_t *nm, uint32_t *ne)
{
  *nk = ht->n_keys;
  *nh = ht->n_hits;
  *nm = ht->n_misses;
  *ne = ht->n_examined;
}
