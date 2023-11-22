#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include "parser.h"

#define MAX_PARSER_LINE_LENGTH 1000

/* Returns the string length of word */
static uint64_t string_len(char *word)
{
  uint64_t length = 0;
  while (word[length] != '\0')
  {
    length++;
  }
  return length;
}

/* Copies the string s2 into string s1 */
static void string_cpy(char *s1, char *s2)
{
  uint64_t index = 0;
  while (s2[index] != '\0')
  {
    s1[index] = s2[index];
    index++;
  }
  s1[index] = '\0';
}

/* The attributes of a Parser object */
struct Parser {
  FILE *f;
  char current_line[MAX_PARSER_LINE_LENGTH + 1];
  uint32_t line_offset;
};

/* Creates a Parser object and its attributes. Sets the string
current_line with the characters read from the FILE object *f */
Parser *parser_create(FILE *f) {

  Parser *pa = (Parser *)malloc(sizeof(Parser));
  pa->f = f;
  fgets(pa->current_line, 1001, f);
  pa->line_offset = 0;
  return pa;
}

/* Frees memory from the Parser object */
void parser_delete(Parser **p) {
  free((*p));
}

/* Set *word with the next valid word in the buffer within the parser. Keep
in mind that a valid word contains characters that are alphamuneric,
hyphens, or a apostrophe. Returns true if there are still valid words and
return false if there aren't. */
bool next_word(Parser *p, char *word){

  if (p->line_offset >= string_len(p->current_line))
  {
    return false;
  }
  
  uint32_t new_offset = 0;
  
  if (p->current_line[p->line_offset] != '-' && p->current_line[p->line_offset] != '\'' && isalnum(p->current_line[p->line_offset]) == 0)
  {
    while (p->current_line[p->line_offset] != '-' && p->current_line[p->line_offset] != '\'' && isalnum(p->current_line[p->line_offset]) == 0)
    {
      if (p->current_line[p->line_offset] == '\0')
      {
        return false;
      }
      p->line_offset += 1;
    }
  }
  
  
  if (p->line_offset >= string_len(p->current_line))
  {
    return false;
  }
  
  for (uint32_t i = p->line_offset; i <= string_len(p->current_line); i++)
  {
    if (p->current_line[i] != '-' && p->current_line[i] != '\'' && isalnum(p->current_line[i]) == 0)
    {
      new_offset = i + 1;
      break;
    }
  }
  
  
  char str[1050];
  uint64_t index = 0;
  
  for (uint64_t i = p->line_offset; i < (new_offset - 1); i++)
  {
    str[index] = tolower(p->current_line[i]);
    index++;
  }
  str[index] = '\0';
  
  string_cpy(word, str);

  p->line_offset = new_offset;
 
  
  return true;
}
