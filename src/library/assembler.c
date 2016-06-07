#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "assembler.h"
#include "tokens.h"
#include "bitwise.h"
#include "symbolTableList.h"


extern ASSEMBLER_STRUCT *ass;


void assemble_free(ASSEMBLER_STRUCT *ass)
{

  for(int i = 0; i < ass->TOTAL_line; i++) {
    //puts("in for loop in ass free");
    free(ass->instr[i]);
  }
  //printf("after for loop in assemble_free\n");

  //free(ass->instr);

    //list_destroy(ass->symbolTable);

  free(ass);
}

/*static void realloc_instrs(ASSEMBLER_STRUCT *ass)
{
  ass->instr = realloc(ass->instr, sizeof(binary_instruct *) * (ass->TOTAL_line));
}*/

uint16_t assemble_constant_write(ASSEMBLER_STRUCT *ass, int32_t word)
{
    ass->TOTAL_line++;

    ass->instr = realloc(ass->instr, sizeof(binary_instruct*) * ass->TOTAL_line);

	uint16_t address       = (ass->TOTAL_line-1) * sizeof(int32_t);
	binary_instruct *instr = malloc(sizeof(binary_instruct));
	instr->binary_word     = word;
	instr->word_address    = address;

	int current_instruct         = address / sizeof(int32_t);
	ass->instr[current_instruct] = instr;

	return address;
}


void assemble_write(ASSEMBLER_STRUCT *ass, int32_t word)
{
  //printf("%08x: ", ass->current_address);
  //print_bits_inBE(word);

  binary_instruct *instr = malloc(sizeof(binary_instruct));

  instr->binary_word = word;
  //printf("current addr :%" PRIu32 "\n", ass->current_address);
  instr->word_address = ass->current_address; //0
  int current_instruct = ass->current_address / sizeof(int32_t); //0
  //printf("current instruct: %i\n", current_instruct);
  ass->instr[current_instruct] = instr;
  ass->current_address += sizeof(int32_t);
  //printf("current addr after add:%" PRIu32 "\n", ass->current_address);

}


int32_t *assemble_generate_bin(ASSEMBLER_STRUCT *ass) {

  int32_t *words = malloc(sizeof(int32_t) * (ass->TOTAL_line));
  //printf("before for loop in generate_bin\n");
  for(int i = 0; i < (ass->TOTAL_line); i++)
  {
    //printf("%i\n", ass->TOTAL_line );
    //printf("ass->instr[i]->binary_word %c\n", (ass->instr[i])->binary_word);
    words[i] = (ass->instr[i])->binary_word;
    //printf("words[i] after writing....%i\n",words[i]);
   //printf("words generate: %i \n", words[i]);
  }

  return words;
}

char *delchr(char *buffer, char chr) {
    char *str = strdup(buffer);
    char* c;

	if ((c = strchr(str, chr)) != NULL) {
	    strcpy(c, c+1);
	}
	return str;
}


ASSEMBLER_STRUCT *assemble(TOKEN *lines, function_assPtr func, const char *delim)
{

  //printf("start assemble\n");
  symbolTableList* symbolTable = malloc(sizeof(symbolTableList));
  //printf("will this work\n");

  list_initialise(symbolTable);

  // 1st Pass : Check for labels
	uint16_t  address        = 0;
	int       label_count    = 0;
	for (int i = 0; i < lines->tokenCount; i++)
	{
		char   *current_Line = strdup(lines->tokens[i]);
    //printf("current line: %s\n", current_Line);
		TOKEN *line          = tokenise(current_Line, delim);
		char   *label        = line->tokens[0];
    //printf("label: %s\n", label);

    if(strchr(label, ':')) { //label encountered
      label_count++;
      if(label_count == 1) { // first label encountered
        //printf("label count is 1\n");
      }
      //printf("go into list insert back\n");
      list_insert_back(symbolTable, delchr(label, ':'), address);
      //printf("after list_insert_back\n");
      //printf("first value in list:%s\n", list_iter_value(ass->symbolTable->first));
      // add symbol at end of list each time

    }
    //printf("notlabel02: %s\n", label);
		address += sizeof(uint16_t);
		tokens_free(line);

  }
  //printf("after for loop\n");


//printf("after for loop in assemble\n");
  // Initialize Assembly Program
	int line_total         = lines->tokenCount - label_count;
	ass->instr             = malloc(sizeof(binary_instruct));
	ass->TOTAL_line        = line_total;
	ass->symbolTable       = symbolTable;
	ass->current_address   = 0;

  //printf("before pass 2\n");
	// Pass #2
	for (int i = 0; i < lines->tokenCount; i++)
	{
		char  *current_Line = strdup(lines->tokens[i]);
    //printf("currl for tokenise: %s\n",current_Line);
		TOKEN *line = tokenise(current_Line, delim);
		char *mnemonic = line->tokens[0];
		if (strchr(mnemonic, ':')) continue; // Label encountered

		int32_t word = func(line, ass);
    //printf("word: %i\n", word );

		assemble_write(ass, word);

    //printf("can i write in assemle????\n");
    //printf("go to token_free\n");
    tokens_free(line);
    //printf("token_freed?\n");
	}
  //printf("assemble done\n");
  //printf("let's return ass\n");
	return ass;


}
