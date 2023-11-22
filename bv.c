#include <stdio.h>
#include <stdlib.h>
#include "bv.h"
#include "bf.h"
#include "city.h"

/* The attributes of a BitVector object */
struct BitVector{
  uint32_t length;
  uint64_t *vector;
};

/* Creates a BitVector object with length amount of bits. If the length 
given is zero or less, the function will return NULL. */
BitVector *bv_create(uint32_t length)
{
  if (length <= 0)
  {
    return NULL;
  }
  
  BitVector *v = (BitVector *)malloc(sizeof(BitVector));
  v->length = length;

  if ((length % 64) != 0)
  {
    v->vector = (uint64_t *)calloc((length / 64) + 1, sizeof(uint64_t));
  }
  else
  {
    v->vector = (uint64_t *)calloc((length / 64), sizeof(uint64_t));
  }
  
  
  if (v->vector == NULL)
  {
    return NULL;
  }
  return v;
}

/* Frees memory from the vector and the BitVector object itself. */
void bv_delete(BitVector **bv) {
  if (*bv != NULL && ((*bv)->vector) != NULL)
  {
    free((*bv)->vector);
  }
  
  if (*bv)
  {
    free(*bv);
    *bv = NULL;
  }
}

/* Returns the amount of bits a BitVector object is currently using. */
uint32_t bv_length(BitVector *bv) {
  return bv->length;
}

/* Sets a specific bit i in the BitVector object to 1*/
void bv_set_bit(BitVector *bv, uint32_t i) {
  if (bv != NULL && (bv->vector) != NULL)
  {
    bv->vector[i / 64] |= ((uint64_t)1 << (i % 64));
  }
}

/* Sets a specific bit i in the BitVector object to 0*/
void bv_clr_bit(BitVector *bv, uint32_t i) {
  if (bv != NULL && (bv->vector) != NULL)
  {
    bv->vector[i / 64] &= ~((uint64_t)1 << (i % 64));
  }
}

/* Returns the value of a specific bit i in the BitVector object*/
uint8_t bv_get_bit(BitVector *bv, uint32_t i) {
  if (((bv->vector[i / 64] >> (i % 64)) & (uint64_t)0x1) != 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/* Prints out all the bits in the BitVector object. Since the BitVector
object is intended to hold uint64_t integers, 64 bits will be printed per
line until the length of the BitVector object is reached. */
void bv_print(BitVector *bv) {

  uint64_t vector_amount = 0;
  if ((bv_length(bv) % 64) != 0)
  {
    vector_amount = (bv_length(bv) / 64) + 1;
  }
  else
  {
    vector_amount = (bv_length(bv) / 64);
  }
  
  
  for (uint64_t i = 0; i < (64 * vector_amount); i++)
  {
    if (i != 0 && (i % 64 == 0))
    {
      printf("\n");
    }
    printf("%d", bv_get_bit(bv,i));
  }
  printf("\n");
}
