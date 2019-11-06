// Compiler Builder 25
// Elizabeth Maspoch
// Hemali Mistry
// Kevin Negy
// Courtney Tennant

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"


#define MAX_CODE_LENGTH 500
#define MAX_SYMBOL_TABLE_SIZE 100

struct pm0_code{
    int op;
    int l;
    int m;
};

typedef struct symbol {
 int kind; // const = 1, var = 2, proc = 3
 char name[12]; // name up to 11 chars
 int val; // number (ASCII value)
 int level; // L level
 int addr; // M address
} symbol;


/*** Global Variables ***/


// Lexer.c variables
FILE* fp;
char** tokens;
int* tokenType;
int error, errorfound, currentToken, size;

// Arrays to store the symbol table and the code generation
struct pm0_code code[MAX_CODE_LENGTH];
symbol symbol_Table[MAX_SYMBOL_TABLE_SIZE];

// Cx is current code index
int cx=0, tokenIndex=0, level=0, symbolIndex=0;


/*** Function signatures ***/


void ERROR(char error[]);
void getToken();

// Symbol table
void ENTER(int kind, char name[],int val, int level, int addr);
int findIndex(char token[]);
void print_symbol_table();

// Code generation
void gen(int index, int operation, int lvl, int value);
void print_code_file();
void print_code();

// Recursive descent parser functions
void PROGRAM();
void BLOCK();
void STATEMENT();
void CONDITION ();
void EXPRESSION ();
void TERM ();
void FACTOR ();

int convert_to_num(char num[]);


/***   ***/


int main(int argv,char* args[]){

    if(argv != 3){
        printf("./compile <input> <output>\n");
        return -1;
    }

    int i;
    char temp[13];

    // Executes lexer and creates the token array
    lex(argv,args);
    tokenIndex=0;

    // Starts parsing and code generation
    PROGRAM();

    // No errors found if this line is reached
    printf("No errors, program is syntactically correct\n");

    // Create an output file with the code generated
    print_code_file(args);

    // Free all allocated arrays
    FreeTokenArrays();
    return 0;
}

// Prints error and terminates the entire program
void ERROR(char error[]){
    printf("%s\n", error);
    exit(0);
}

// Increments token index so that the next token can be processed
void getToken(){
    tokenIndex++;
    return;
}


/*** SYMBOL TABLE FUNCTIONS ***/


// Takes in a symbol and its characteristics and stores into the symbol table
void ENTER(int kind, char name[],int val, int level, int addr){
    // Check for array out of bounds
    if(symbolIndex>=MAX_SYMBOL_TABLE_SIZE){
        printf("Not enough memory allocated for symbol_Table - Array out of bounds in ENTER function");
        exit(0);
    }

    symbol_Table[symbolIndex].kind=kind;
    strcpy(symbol_Table[symbolIndex].name, name);
    symbol_Table[symbolIndex].addr = addr;
    symbol_Table[symbolIndex].val=val;
    symbol_Table[symbolIndex].level=level;

    // Increment the current symbol index number
    symbolIndex++;

}

// Takes in a string and returns the index where the string is located in the symbol table
int findIndex(char token[]){

    int i;
    for (i = 0; i <MAX_SYMBOL_TABLE_SIZE; i++) {

        // return the index i if const,var or proc has been declared
        if (strcmp(symbol_Table[i].name, token) == 0)
            return i;
    }
    // never found
    return -1;
}

// Helper function that prints out the contents of symbol table
void print_symbol_table(){
    int i=0;
    for(i=0;i<symbolIndex;i++)
         printf("%d %13s %d %d %d\n",symbol_Table[i].kind, symbol_Table[i].name,symbol_Table[i].val,symbol_Table[i].level,symbol_Table[i].addr );
}


/*** CODE GENERATION HELPER FUNCTIONS ***/


// Inserts code into the code array
void gen(int index, int operation, int lvl, int value){

    if(index>MAX_CODE_LENGTH-1 ||index<0){
        printf("Error: gen function index out of range");
        return;
    }

    code[index].op=operation;
    code[index].l=lvl;
    code[index].m=value;

    // if cx is used as current index, increment current index. Otherwise, leave cx as is
    if(index==cx)
        cx++;
    return;
}

// Creates an output file titled with the name of second command line argument and prints all pm0 code to it
void print_code_file(char* args[]){

    FILE *fp;
    fp=fopen(args[2],"w");
    int i=0;

    while(code[i].op!=9||code[i].l!=0||code[i].m!=2){
        fprintf(fp,"%d %d %d\n",code[i].op,code[i].l,code[i].m);
        i++;
    }

    // Halt code found, print the halt code out and close file
    fprintf(fp,"%d %d %d",code[i].op,code[i].l,code[i].m);
    fclose(fp);
}

// Prints code to standard output
void print_code(){

    int i=0;
    while(code[i].op!=9||code[i].l!=0||code[i].m!=2){
        printf("%d %d %d\n",code[i].op,code[i].l,code[i].m);
        i++;
    }

    // Halt code found, print the halt code out
    printf("%d %d %d",code[i].op,code[i].l,code[i].m);
}


/*** RECURSIVE DESCENT PARSE CODE WITH CODE GENERATION INTERWOVEN ***/


void PROGRAM(){

    BLOCK();
    if(tokenType[tokenIndex]!=periodsym)
        ERROR("Error number 9: Period expected!");

    // Create HALT code
    gen(cx,9,0,2);
}

void BLOCK(){

     // Create JMP code to go to end of procedure until procedure is called (address is changed after BLOCK() call)
    int jmp_cx=cx;
    gen(cx, 7, 0, 0);

    // Constant symbol block
    if (tokenType[tokenIndex]==constsym){

        char const_name[13];
        int number=0;

        do{
            getToken();
            if(tokenType[tokenIndex]!=identsym)
                ERROR("Error number 4: Constant has to be followed by an identifier.");


            // Store the current constant identifier
            strcpy(const_name,tokens[tokenIndex]);
            getToken();
            if(tokenType[tokenIndex]!=eqsym)
                ERROR("Error number 1: Must use = instead of :=");

            getToken();
            if(tokenType[tokenIndex]!=numbersym)
                ERROR("Error number 3: = Must be followed by a number");

            // Get number to assign to the constant identifier and enter into symbol table
            number=convert_to_num(tokens[tokenIndex]);
            ENTER(1,const_name,number,level,0);
            getToken();

        }while(tokenType[tokenIndex]==commasym);

        if(tokenType[tokenIndex]!=semicolonsym)
                ERROR("Error number 5: Semicolon or comma is missing.");

        getToken();
    }

    // Variable symbol block
    int num_vars=0;
    if (tokenType[tokenIndex]==varsym){

        do{
            getToken();
            if(tokenType[tokenIndex]!=identsym)
                ERROR("Error number 4: Var must be followed by an identifier");

            // Keep track of how many variables have been declared
            num_vars++;

            // Enter variable into symbol table with default value 0
            ENTER(2,tokens[tokenIndex],0,level,3+num_vars);
            getToken();

        }while(tokenType[tokenIndex]==commasym);

        if(tokenType[tokenIndex]!=semicolonsym)
                ERROR("Error number 5: Semicolon or comma is missing.");

        getToken();
    }

    // Procedure symbol block
    while(tokenType[tokenIndex]==procsym){

        // At the start of a new procedure, increment level. Once the recursion returns from BLOCK(), decrement level
        level++;
        getToken();

        if(tokenType[tokenIndex]!=identsym)
               ERROR("Error number 4: Procedure be followed by an identifier.");

        // Enter procedure identifier into symbol table
        ENTER(3,tokens[tokenIndex],0,level,cx);
        getToken();

        if(tokenType[tokenIndex]!=semicolonsym)
             ERROR("Error number 6: Incorrect symbol after procedure declaration.");
        getToken();

        BLOCK();

         // Generate return code
        gen(cx,2,0,0);

        if(tokenType[tokenIndex]!=semicolonsym)
              ERROR("Error number 5: Semicolon or comma is missing.");

        getToken();
        level--;

    }
        // Change JMP command to correct address
        code[jmp_cx].m=cx;

    // After variables declared, create INC code that allocates M locals on stack
    // M = 4 for function values plus num_vars to store the variables
    gen(cx,6,0,4+num_vars);

    // End of block function must call statement function
    STATEMENT () ;

}

void STATEMENT(){

    // Identifier found at beginning of statement
    if (tokenType[tokenIndex] == identsym ){

        // Store symbol table index of current identifier
        int tok_index=findIndex(tokens[tokenIndex]);

        // Identifier not in symbol table
        if(tok_index == -1)
            ERROR("Error number 11: Undeclared identifier.");

        if(symbol_Table[tok_index].kind!=2)
            ERROR("Error number 12: Assignment to constant or procedure is not allowed");
        getToken();

        if(tokenType[tokenIndex] != becomessym)
            ERROR("Error number 13: Assignment operator expected");
        getToken();

        // Call expression to find value of the variable, create STO code to store value into variable, uses level equation: level-variable level
        EXPRESSION ();
        gen(cx,4, level-symbol_Table[tok_index].level,symbol_Table[tok_index].addr);
    }

    // Call symbol statement
    else if (tokenType[tokenIndex] == callsym){

        getToken();
        if (tokenType[tokenIndex] != identsym)
            ERROR("Error number 14: Call must be followed by an identifier.");

        int proc_index=findIndex(tokens[tokenIndex]);

        // Creates CAL code using the level equation: 1+(level - procedure.level)
        gen(cx,5, 1+level-symbol_Table[proc_index].level, symbol_Table[proc_index].addr);

        getToken();

    }

    // Begin symbol statement
    else if(tokenType[tokenIndex]==beginsym){

        getToken();
        STATEMENT ();
        while (tokenType[tokenIndex]==semicolonsym){
            getToken();
            STATEMENT();
        }

        if (tokenType[tokenIndex] != endsym )
            ERROR("Error number 31: End is expected!");
        getToken();
    }

    // If symbol statement
    else if (tokenType[tokenIndex] == ifsym){

        getToken();
        CONDITION ();

        if (tokenType[tokenIndex] != thensym)
           ERROR("Error number 16: Then expected!");
        getToken();

        // Store current code index, create JPC code whose M value will be modified after STATEMENT call
        int temp=cx;
        gen(cx,8,0,0);
        STATEMENT ();

        // Checks if there is an else code after the if code
        if(tokenType[tokenIndex] == elsesym) {

            getToken();
            int jmp_cx = cx;

            // Create JMP code
            gen (cx,7,0,0);

            // Modify JPC address to jump to current address if else statement is executed
            code[temp].m=cx;

            STATEMENT ();

            // Modify JMP code to jump to current address
            code[jmp_cx].m = cx;
        }
        else

            code[temp].m=cx;
    }

    // While symbol statement
    else if (tokenType[tokenIndex] == whilesym){

        // Store current code index to use for JPC function below
        int before=cx;
        getToken();

        CONDITION();

        // Store current code index to use for JMP function below
        int after=cx;

        // JMP instruction will be stored at that current index below, move to next cx
        cx++;

        if (tokenType[tokenIndex] != dosym)
             ERROR("Error number 18: Do expected!");
        getToken();

        STATEMENT ();

        // JPC function to redo while loop
        gen(cx,7,0,before);

        // JMP function to skip while code once condition fails
        gen(after,8,0,cx);

    }

    // Read symbol statement
    else if(tokenType[tokenIndex]==readsym){

        getToken();
        if(tokenType[tokenIndex]!=identsym)
            ERROR("Error number 29: Expected identifier.");

        // Get index for variable identifier in symbol table
        int index=findIndex(tokens[tokenIndex]);

        // Identifier not in symbol table
        if(index == -1)
            ERROR("Error number 11: Undeclared identifier.");

        // SIO INPUT to scan in number
        gen(cx, 9,0,1);

        // STO code to store scanned number into the variable identifier
        gen(cx, 4, symbol_Table[index].level,symbol_Table[index].addr);

        getToken();
    }

    // Write symbol statement
    else if(tokenType[tokenIndex]==writesym){

        getToken();
        if(tokenType[tokenIndex]!=identsym)
            ERROR("Error number 29: Expected identifier.");

        // Get index for variable identifier in symbol table
        int index=findIndex(tokens[tokenIndex]);

        // Identifier not in symbol table
        if(index == -1)
            ERROR("Error number 11: Undeclared identifier.");

        // LOD code to get value of variable
        if(symbol_Table[index].kind==2)
            gen(cx, 3,symbol_Table[index].level,symbol_Table[index].addr);

        // LIT code to put value of constant on stack
        if(symbol_Table[index].kind==1)
            gen(cx, 1,symbol_Table[index].level,symbol_Table[index].val);

        // SIO OUTPUT code to pop the stack and write the value to standard output
        gen(cx,9,0,0);

        getToken();
    }
}

void CONDITION () {

    if (tokenType[tokenIndex]== oddsym){
        getToken();
        EXPRESSION ();

        // OPR ODD code generation
        gen(cx,2,0,6);
    }

    else{
        EXPRESSION ();

        // Conditional relation statement
        if (tokenType[tokenIndex] > 15 || tokenType[tokenIndex]<8)
            ERROR("Error number 20: Relational operator expected.");

        // Store index of relational operator
        int temp=tokenIndex;

        getToken();
        EXPRESSION();

        // OPR condition relation code. Relational operator number in tokenType is one more than corresponding pm0 M value
        gen(cx, 2,0,tokenType[temp]-1);
    }
    return;
}

void EXPRESSION () {

    // Store current token index
    int temp=tokenIndex;

    // Addition, subtraction, or negation symbol present
    if (tokenType[tokenIndex] == plussym || tokenType[tokenIndex]==minussym)
        getToken();

    // TERM mandatory call
    TERM();

    // If initial token in expression was a minus, it is actually a negate symbol. Create OPR NEGATE code
    if(tokenType[temp]==minussym)
        gen(cx,2,0,1);

    // If expression was addition or subtraction
    while (tokenType[tokenIndex] == plussym || tokenType[tokenIndex]==minussym){

        temp=tokenIndex;
        getToken ();
        TERM();

        // OPR ADD code
        if(tokenType[temp]==plussym)
            gen(cx,2,0,2);
        // OPR SUB code
        else
            gen(cx,2,0,3);
    }

}

void TERM () {

    // FACTOR mandatory call
    FACTOR ();

    // Multiplication or division operations
    while(tokenType[tokenIndex] == multsym || tokenType[tokenIndex]==slashsym){

        // Store current token index to keep track of which operation to perform
        int temp=tokenIndex;

        getToken();
        FACTOR();

        // OPR MULT code
        if(tokenType[temp]==multsym)
            gen(cx,2,0,4);

        // OPR DIV code
        else
            gen(cx,2,0,5);
    }
    return;
}

void FACTOR () {

    // Identifier factor
    if (tokenType[tokenIndex] == identsym){

        // Get index of identifier in the symbol table
        int temp=findIndex(tokens[tokenIndex]);

        // If variable identifier, LOD command, uses level equation: level-variable level
        if(symbol_Table[temp].kind==2)
            gen(cx, 3, level-symbol_Table[temp].level,symbol_Table[temp].addr);

        // If constant identifier, LIT command
        else if (symbol_Table[temp].kind==1)
            gen(cx,1,0,symbol_Table[temp].val);

        // If procedure identifier
        else if(symbol_Table[temp].kind==3)
            ERROR("Error number 30: Factor must not contain a procedure identifier.");

        // Identifier not in symbol table
        else
            ERROR("Error number 11: Undeclared identifier.");
        getToken();
    }

    // Number symbol factor
    else if (tokenType[tokenIndex] == numbersym){

        // Convert string to integer and create LIT code
        int number=convert_to_num(tokens[tokenIndex]);
        gen(cx,1,0,number);

        getToken ();
    }

    // Parenthesis factor
    else if (tokenType[tokenIndex] == lparentsym){

        getToken();
        EXPRESSION();

        if (tokenType[tokenIndex] != rparentsym )
            ERROR("Error number 22: Right parenthesis missing.");

        getToken();
    }
}


/*** END RECURSIVE DESCENT PARSER AND CODE GENERATION FUNCTIONS***/


// Takes in a string with number symbols and returns the integer value of the string
int convert_to_num(char num[]){

    int length=strlen(num);
    int number=0,i,j;
    int power=1;
    for(i=0,j=length-1;i<length;i++,j--){
            number+= ((num[j]-'0')*power);
            power*=10;
    }
    return number;
}

