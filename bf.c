#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "bv.h"
#include "bf.h"
#include "city.h"

#define N_HASHES 5

/* The attributes of a BitVector object */
struct BitVector{
  uint32_t length;
  uint64_t *vector;
};

/* The attributes of a BloomFilter object */
struct BloomFilter {
  uint64_t salts[N_HASHES];
  uint32_t n_keys;
  uint32_t n_hits;
  uint32_t n_misses;
  uint32_t n_bits_examined;
  BitVector * filter;
}; 

/* The 5 default salt values from the BloomFilter object that are going
to be utilized later for hashing and looking-up values. */
static uint64_t default_salts [] = {
  0x5adf08ae86d36f21,
  0x419d292ea2ffd49e,
  0x50d8bb08de3818df,
  0x272347aea4045dd5,
  0x7c8e16f768811a21
};

/* Creates the BloomFilter object and its attributes */
BloomFilter *bf_create(uint32_t size){
  
  BloomFilter *bf = (BloomFilter *)malloc(sizeof(BloomFilter));
  
  if (bf)
  {
    bf->n_keys = bf->n_hits = 0;
    bf->n_misses = bf->n_bits_examined = 0;
    
    for (int i = 0; i < N_HASHES; i++)
    {
      bf->salts[i] = default_salts[i];
    }
    
    bf->filter = bv_create(size);
    
    if (bf->filter == NULL)
    {
      free(bf);
      bf = NULL;
    }
  }
  
  return bf;
}

/* Frees the BloomFilter object's filter and itself */
void bf_delete(BloomFilter **bf){
  if (*bf != NULL && ((*bf)->filter) != NULL)
  {
    bv_delete(&((*bf)->filter));
  }
  
  if (*bf)
  {
    free(*bf);
    *bf = NULL;
  }
}

/* Returns the size of the BloomFilter object */
uint32_t bf_size(BloomFilter *bf){
  return bv_length(bf->filter);
}

/* Inserts the oldspeak word to the BloomFilter object with the 5 default
salt values, the hash function from CityHash, and the size of the
Bloomfilter object itself. */
void bf_insert(BloomFilter *bf, char *oldspeak)
{
  bv_set_bit(bf->filter, (hash(bf->salts[0], oldspeak) % bf_size(bf)));
  bv_set_bit(bf->filter, (hash(bf->salts[1], oldspeak) % bf_size(bf)));
  bv_set_bit(bf->filter, (hash(bf->salts[2], oldspeak) % bf_size(bf)));
  bv_set_bit(bf->filter, (hash(bf->salts[3], oldspeak) % bf_size(bf)));
  bv_set_bit(bf->filter, (hash(bf->salts[4], oldspeak) % bf_size(bf)));
  bf->n_keys += 1;
}

/* Checks to see if an oldspeak word is currently present in the filter */
bool bf_probe(BloomFilter *bf, char *oldspeak)
{
  for (uint8_t i = 0; i < 5; i++)
  {
    uint8_t result_bit = bv_get_bit(bf->filter, 
    (hash(bf->salts[i], oldspeak) % bf_size(bf)));
    
    bf->n_bits_examined += 1;
    
    if (result_bit != 1)
    {
      bf->n_misses += 1;
      return false;
    }
  }
  bf->n_hits += 1;
  return true;
}

/* Returns the amount of bits set to 1 in the BloomFilter object's filter */
uint32_t bf_count(BloomFilter *bf) {
  uint32_t count = 0;
  for (uint32_t i = 0; i < bf_size(bf); i++)
  {
    if (bv_get_bit(bf->filter,i) == 1)
    {
      count++;
    }
  }
  
  return count;
} 

/* Prints the BloomFilter's filter to standard output */
void bf_print(BloomFilter *bf) {
  bv_print(bf->filter);
}

/* Sets each pointer to the value of the original variable of the relevant
statistic that the BloomFilter is keeping track off.*/
/* nk tracks the number of keys present in the BloomFilter bf */
/* nh tracks the number of hits from all the probes performed with the
BloomFilter object bf */
/* nm tracks the number of misses from all the probes performed with the
BloomFilter object bf */
/* ne tracks the number of bits examined from all the probes performed
with the BloomFilter object bf */
void bf_stats(BloomFilter *bf, uint32_t *nk, uint32_t *nh, uint32_t *nm, uint32_t *ne) {
  *nk = bf->n_keys;
  *nh = bf->n_hits;
  *nm = bf->n_misses;
  *ne = bf->n_bits_examined;
}
