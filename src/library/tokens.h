#ifndef _TOKENS_H
#define _TOKENS_H

#include <stdio.h>
#include <stdlib.h>


///////////////////////////////////////////////////////////////////////////////


typedef struct TOKEN
{
  char **tokens;
  unsigned int tokenCount;
  char *line;
} TOKEN;

typedef void (*tokens_func)(char *);

///////////////////////////////////////////////////////////////////////////////

/*Function Prototype*/

void tokens_free(TOKEN *);

void tokens_print(TOKEN *);

TOKEN tokenise(char* , const char*);

void tokens_iter(TOKEN *, tokens_func);

char tokens_endc(TOKEN *);


//////////////////////////////////////////////////////////////////////////////

/*Parsing Macro*/

#define Is_Expression(token)	(token[0] == '#' || token[0] == '=')
#define Is_Hexadecimal(token) (Is_Expression(token) & token[1] == '0' & token[2] == 'x')
#define expr_to_num(expr)    (strtol(expr, NULL, 0))
#define PARSE_REG(R)		\
           (((R) == (-1)) ? 0 \
		          : ((strcmp(line->tokens[R], "PC") == 0) ? PC \
					: atoi(line->tokens[R] + 1)))

#endif
