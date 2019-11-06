#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

// Compiler Builder 25
// Elizabeth Maspoch
// Hemali Mistry
// Kevin Negy
// Courtney Tennant

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

FILE* fp;
char** tokens;
int* tokenType;
extern int error, errorfound, currentToken,size;


typedef enum token {
  nulsym = 1, identsym, numbersym, plussym, minussym,
  multsym, slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
  gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
  periodsym, becomessym, beginsym, endsym, ifsym, thensym,
  whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
  readsym , elsesym
} token_type;

void lex(int argv,char* args[]);
void FreeTokenArrays();

#endif // LEXER_H_INCLUDED
