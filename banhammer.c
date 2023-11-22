#include "bf.h"
#include "bv.h"
#include "city.h"
#include "ht.h"
#include "ll.h"
#include "messages.h"
#include "node.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#define N_HASHES 5
#define OPTIONS "ht:f:ms"

static uint64_t string_len(char *word)
{
  uint64_t length = 0;
  while (word[length] != '\0')
  {
    length++;
  }
  return length;
}

uint64_t string_cmp(char * first_str, char * second_str)
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

void string_cpy(char *s1, char *s2)
{
  uint64_t index = 0;
  while (s2[index] != '\0')
  {
    s1[index] = s2[index];
    index++;
  }
  s1[index] = '\0';
}

struct BitVector{
  uint32_t length;
  uint64_t *vector;
};

struct BloomFilter {
  uint64_t salts[5];
  uint32_t n_keys;
  uint32_t n_hits;
  uint32_t n_misses;
  uint32_t n_bits_examined;
  BitVector * filter;
};

struct LinkedList {
  uint32_t length;
  Node *head;
  Node *tail;
  bool mtf;
};

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

struct Parser {
  FILE *f;
  char current_line[MAX_PARSER_LINE_LENGTH + 1];
  uint32_t line_offset;
};

int main(int argc, char **argv) {
  
  int opt = 0;
  
  uint32_t hash_size = 10000;
  uint32_t filter_size = 524288;
  uint32_t bf_keys = 0;
  uint32_t bf_hits = 0;
  uint32_t bf_misses = 0;
  uint32_t bf_bits_examined = 0;
  uint32_t ht_keys = 0;
  uint32_t ht_hits = 0;
  uint32_t ht_misses = 0;
  uint32_t ht_examined = 0;
  
  
  bool mtf = false;
  bool activate_stats = false;
  
  
  while ((opt = getopt(argc, argv, OPTIONS)) != -1)
  {
    switch(opt){
    case 'h': /* Prints out program usage */
      fprintf(stderr,"Usage: %s [options]\n", argv[0]);
      fprintf(stderr,"  %s will read in words from stdin, identify any badspeak or old speak and output an\n", argv[0]);
      fprintf(stderr,"  appropriate punishment message. The badspeak and oldspeak (with the newspeak translation)\n");
      fprintf(stderr,"  that caused the punishment will be printed after the message. If statistics are enabled\n");
      fprintf(stderr,"  punishment messages are suppressed and only statistics will be printed.\n");
      fprintf(stderr,"    -t <ht_size>: Hash table size set to <ht_size>. (default: 10000)\n");
      fprintf(stderr,"    -f <bf_size>: Bloom filter size set to <bf_size>. (default 2^19)\n");
      fprintf(stderr,"    -s          : Enables the printing of statistics.\n");
      fprintf(stderr,"    -m          : Enables move-to-front rule.\n");
      fprintf(stderr,"    -h          : Display program synopsis and usage.\n");
      return 0;
      break;
    case 't': /* Specifies the size of the hash table */
      if (atoi(optarg) <= 0)
      {
        fprintf(stderr, "%s: Invalid hash table size.\n", argv[0]);
        return 1;
      }
      hash_size = strtoul(optarg, NULL, 10);
      break;
    case 'f': /* Specifies the size of the Bloom filter */
      if (atoi(optarg) <= 0)
      {
        fprintf(stderr, "%s: Invalid bloom filter table size.\n", argv[0]);
        return 1;
      }
      filter_size = strtoul(optarg, NULL, 10);
      break;
    case 'm': /* Enables the move-to-front rule */
      mtf = true;
      break;
    case 's': /* Enables the prinintning of statistics to stdout */
      activate_stats = true;
      break;
    default: /* Any other options */
      fprintf(stderr,"Usage: %s [options]\n", argv[0]);
      fprintf(stderr,"  %s will read in words from stdin, identify any badspeak or old speak and output an\n", argv[0]);
      fprintf(stderr,"  appropriate punishment message. The badspeak and oldspeak (with the newspeak translation)\n");
      fprintf(stderr,"  that caused the punishment will be printed after the message. If statistics are enabled\n");
      fprintf(stderr,"  punishment messages are suppressed and only statistics will be printed.\n");
      fprintf(stderr,"    -t <ht_size>: Hash table size set to <ht_size>. (default: 10000)\n");
      fprintf(stderr,"    -f <bf_size>: Bloom filter size set to <bf_size>. (default 2^19)\n");
      fprintf(stderr,"    -s          : Enables the printing of statistics.\n");
      fprintf(stderr,"    -m          : Enables move-to-front rule.\n");
      fprintf(stderr,"    -h          : Display program synopsis and usage.\n");
      return 1;
      break;
    }
  }
  
  /* Initializes the hash table and the bloom filter*/
  HashTable *ht = ht_create(hash_size, mtf);
  BloomFilter *bf = bf_create(filter_size);
 
  /* Initializes the linked lists the keep track of badspeak and oldspeak*/
  LinkedList *ultimate_bad_words = ll_create(mtf);
  LinkedList *not_so_bad_words = ll_create(mtf);
 
  /* Associate the bad_speak FILE pointer with "badspeak.txt" */
  FILE* bad_speak;
  bad_speak = fopen("badspeak.txt", "r");
  
  /* Associate the new_speak FILE pointer with "newspeak.txt" */
  FILE* new_speak;
  new_speak = fopen("newspeak.txt", "r");
  
  Parser *ns = parser_create(new_speak); /* Parser for new_speak */
  Parser *input = parser_create(stdin); /* Parser for standard input */
  
  bool first_turn = true;
  char word0[1001];
  char word[1001];
  char word2[1001];
  char word3[1001];
  
  /* Reads from badspeak.txt and putting the badspeak words to the
  bloom filter and the hash table */
  uint64_t len = 0;
  while (fgets(word0, 1001, bad_speak))
  {
    len = string_len(word0);
    if (word0[len - 1] == '\n')
    {
      word0[len - 1] = 0;
    }
    bf_insert(bf, word0);
    ht_insert(ht, word0, NULL);
  }
  
  /* Reads from newspeak.txt and putting the oldspeak words and their
  newspeak translations to the bloom filter and the hash table */
  while (string_cmp(ns->current_line,"") != 0)
  {
    while (next_word(ns, word2) != false)
    {
      if (first_turn == true)
      {
        first_turn = false;
        string_cpy(word, word2);
      }
      else
      {
        first_turn = true;
        bf_insert(bf, word);
        ht_insert(ht, word, word2);
      }
    }
    parser_delete(&ns);
    ns = parser_create(new_speak);
  }
  
  /* Reads from stdin and putting any badspeak words to ultimate_bad_words
  and any oldspeak words with newspeak translations to not_so_bad_words*/
  while (string_cmp(input->current_line,"") != 0)
  {
    while (next_word(input, word3) != false)
    {
      
      if (bf_probe(bf, word3) == true)
      {
        Node *n = ht_lookup(ht, word3);
        
        if (n != NULL)
        {
          if (n->newspeak != NULL)
          {
            ll_insert(not_so_bad_words, n->oldspeak, n->newspeak);
          }
          else
          {
            ll_insert(ultimate_bad_words, n->oldspeak, NULL);
          }
        }
      }
    }
    parser_delete(&input);
    input = parser_create(stdin);
  }

  /* In the case that there are any badspeak and oldspeak words from
  standard input, banhammer.c will print out a mixspeak message and all
  the badspeak and oldspeak words with their newspeak translations. */
  if(ll_length(not_so_bad_words) != 0 && ll_length(ultimate_bad_words) != 0
  && activate_stats == false)
  {
    fprintf(stdout,"%s",mixspeak_message);
    ll_print(ultimate_bad_words);
    ll_print(not_so_bad_words);
  }
  /* In the case that there are any badspeak words from standard input,
  banhammer.c will print out a badspeak message and all
  the badspeak words. */
  else if (ll_length(ultimate_bad_words) != 0 && activate_stats == false)
  {
    fprintf(stdout,"%s",badspeak_message);
    ll_print(ultimate_bad_words);
  }
  /* In the case that there are any newspeak words from standard input,
  banhammer.c will print out a goodspeak message and all
  the oldspeak words and its newspeak translations. */
  else if (ll_length(not_so_bad_words) != 0 && activate_stats == false)
  {
    fprintf(stdout,"%s",goodspeak_message);
    ll_print(not_so_bad_words);
  }
  
  /* Gets statistics related to the bloom filter and the hash table */
  bf_stats(bf, &bf_keys, &bf_hits, &bf_misses, &bf_bits_examined);
  ht_stats(ht, &ht_keys, &ht_hits, &ht_misses, &ht_examined);
  
  /* If activate_stats is set to true, the program will print stats
  related to the bloom filter and the hash table. */
  if (activate_stats == true)
  {
    long double bits_p_miss = 0;
    if (bf_misses == 0)
    {
      bits_p_miss = (bf_bits_examined - (N_HASHES * bf_hits)) 
      /(long double)1;
    }
    else
    {
      bits_p_miss = (bf_bits_examined - (N_HASHES * bf_hits))
      /(long double)(bf_misses);
    }
    
    long double false_positives = 0;
    if (bf_hits == 0)
    {
      false_positives = ((long double)ht_misses / 1.0);
    }
    else
    {
      false_positives = ((long double)ht_misses / bf_hits);
    }
    
    
    
    long double avg_seek_length = 0;
    if (ht_hits + ht_misses == 0)
    {
      avg_seek_length = (((long double)ht_examined) / 1.0);
    }
    else
    {
      avg_seek_length = ((long double)(ht_examined) / 
      (long double)(ht_hits + ht_misses));
    }
    
    long double filter_load = ((long double)bf_count(bf) / (long double)bf_size(bf));
    
    fprintf(stdout, "ht keys: %u\n", ht_keys);
    fprintf(stdout, "ht hits: %u\n", ht_hits);
    fprintf(stdout, "ht misses: %u\n", ht_misses);
    fprintf(stdout, "ht probes: %u\n", ht_examined);
    fprintf(stdout, "bf keys: %u\n", bf_keys);
    fprintf(stdout, "bf hits: %u\n", bf_hits);
    fprintf(stdout, "bf misses: %u\n", bf_misses);
    fprintf(stdout, "bf bits examined: %u\n", bf_bits_examined);
    fprintf(stdout, "Bits examined per miss: %LF\n", bits_p_miss);
    fprintf(stdout, "False positives: %LF\n", false_positives);
    fprintf(stdout, "Average seek length: %LF\n", avg_seek_length);
    fprintf(stdout, "Bloom filter load: %LF\n", filter_load);
  }
  
  /* Frees the bloom filter, hash table, linked lists, parsers, and files */
  ll_delete(&ultimate_bad_words);
  ll_delete(&not_so_bad_words);
  ht_delete(&ht);
  bf_delete(&bf);
  parser_delete(&ns);
  parser_delete(&input);
  free(bad_speak);
  free(new_speak);
  
  
  return 0;
}
