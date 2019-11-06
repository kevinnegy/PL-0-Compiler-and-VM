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
#include "lexer.h"


FILE* fp;
char** error_tokens;
int* error_numbers;
int error, size=500;
int error, errorfound, currentToken, size;
char** tokens;
int* tokenType;


void del_comments(char* text,int end);
void constraints(int argv,char* args[], char* src,char* text, int end);
void print_function(char* text, int end);
void parse_code(char* text, int final);
char* substring(char* text,int begin, int end);
int convert(char* symbol, int length);
int is_operator(char first, char second);
void error_detector(char* symbol, int length, int value);
void process_symbols(char* token,int length);
int string_to_int(char* symbol, int length);
void store_token(char* symbol,int length,int value);
void error_list(char* token, int length, int error_type);
void printerrors();
void lex(int argv,char* args[]);
void FreeTokenArrays();



void lex(int argv,char* args[]){
    errorfound = 0;
    currentToken=0;

    // Initialize error variables for other function use
    error = 0;
    tokenType = malloc(sizeof(int)*size);
    tokens = malloc(sizeof(char*)*size);

    // Opens text file with source code to parse, scans code into src character array
    fp=fopen(args[1],"r");
    char* src=malloc((sizeof(char))*size);
    int i=0;
    while( !feof(fp) ){
         fscanf(fp,"%c",&src[i]);
         if(i==size-1){
            src=realloc(src,size*2);
            size=size*2;
         }
         i++;
    }
    int end=i-2;

    // Reset size value
    size=500;

    // Creates a copy of the source code so that comments can be deleted
    char *text=malloc((sizeof(char))*size);
    text=strcpy(text,src);
    del_comments(text, end);

    // Determines whether to print source code, clean source code, or both
    constraints(argv, args, src, text, end);

    // Splits up the code into tokens and makes further calls to other necessary functions
    parse_code(text, end);
}

// Takes in character array and the final index and deletes all comments
void del_comments(char* text,int end){
    int i=0;
    for(i=0; i<end; i++){
        if(text[i]=='/'&& text[i+1]=='*'){
            while(text[i]!='*'||text[i+1]!='/'){
                text[i]=' ';
                i++;
            }
            text[i]=text[i+1]=' ';
        }
    }
}

//Takes in command line arguments and both source codes and determines which source code to print
void constraints(int argv,char* args[],char* src, char* text, int end){
    int i;
    for(i=0; i<argv; i++){

        //if '--source' is specified as a cmd line arg print source code
        if(strcmp(args[i],"--source")==0){
            printf("\nsource code:\n------------\n");
            print_function(src, end);
            printf("\n");
        }
        //if '--clean is specified as a cmd line arg print source code without comments
        else if(strcmp(args[i],"--clean")==0){
            printf("\nsource code without comments:\n-----------------------------\n");
            print_function(text,end);
            printf("\n");
        }
    }

}

// Takes in character array and final index and prints out entire character array
void print_function(char* text, int end){
    int i;
    for(i=0;i<=end;i++){
        printf("%c",text[i]);
    }
}

// Takes in character array and splits up the tokens. Then calls appropriate token functions
void parse_code(char* text, int final){
    int i=0;
    int begin,end;
    /*printf("tokens:\n-------\n");*/

    while(i<=final){

        // While the index is a space, tab, or new line, continue incrementing i
        while(text[i]==' '||text[i]==9||text[i]=='\n')
            i++;

        // When first non white space symbol found, this is the beginning of a new token
        begin=i;

        // If isalnum==0, the index is a symbol and the token will be a group of symbols starting with this index
        if(isalnum(text[i])==0){
            do{
                i++;
                }while(isalnum(text[i])==0&&text[i]!=' '&&text[i]!=9&&text[i]!='\n'&&text[i]!=3&&text[i]!=2&&i<final+1);
        }

        // Otherwise, the character is alphanumeric and the token will be a group of alphanumerics
        else
            while(isalnum(text[i])!=0&&text[i]!=' '&&text[i]!=9&&text[i]!='\n'&&text[i]!=3&&text[i]!=2){

                // If these symbols are found, do not include in the alphanumeric token
                if(text[i]==','||text[i]==';'||text[i]=='.'||text[i]==')'||text[i]=='('||i>=final+1)
                    break;
                i++;
            }
        end=i;

        // Create new token character array that holds only the token
        char* token=substring(text,begin,end);

        // If the token is an alphanumeric token, call the convert function, then the error_detector function
        if(isalnum(token[0])!=0){
            int value=convert(token,end-begin);
            error_detector(token, end-begin,value);
        }

        // Otherwise, call the process_symbols function on the token made up of symbols
        else{
            // Except if the token is a white space character, do not process
            if(strcmp("\n",token)==0||strcmp("\t",token)==0||strcmp("\0",token)==0)
                continue;
            process_symbols(token, end-begin);
        }

    }
}

// Creates a substring using a given character array and the start and end indexes (end is not included)
char* substring(char* text,int begin, int end){

    int j,k=0;
    char* token=malloc(sizeof(char)*(end-begin+1));
    if(token==NULL)
        printf("RAN OUT OF MEMORY");

    // Copies the characters from text into the new token
    for(j=begin;j<end||k<(end-begin);j++){
        token[k]=text[j];
        k++;
    }
    token[k]='\0';
    return token;
}

// Takes in a character array and determines its token type. Returns the token type (an int)
int convert(char* symbol, int length){

    // Calls is_operator and if it is not an operator, determines which keyword it is or if it is an identifier
    if(is_operator(symbol[0],symbol[1])==0){
        if(strcmp(symbol,"const")==0)
            return constsym;
        else if(strcmp(symbol,"var")==0)
            return varsym;
        else if(strcmp(symbol,"procedure")==0)
            return procsym;
        else if(strcmp(symbol,"call")==0)
            return callsym;
        else if(strcmp(symbol,"begin")==0)
            return beginsym;
        else if(strcmp(symbol,"end")==0)
            return endsym;
        else if(strcmp(symbol,"if")==0)
            return ifsym;
        else if(strcmp(symbol,"then")==0)
            return thensym;
        else if(strcmp(symbol,"else")==0)
            return elsesym;
        else if(strcmp(symbol,"while")==0)
            return whilesym;
        else if(strcmp(symbol,"do")==0)
            return dosym;
        else if(strcmp(symbol,"read")==0)
            return readsym;
        else if(strcmp(symbol,"write")==0)
            return writesym;
        else if(strcmp(symbol,"odd")==0)
            return oddsym;
        // if string_to_int returns NULL, the string is not a number, label as identifier
        else if(string_to_int(symbol, length)== INT_MIN)
                return identsym;
        else
            return numbersym;
    }

    // All operators
    else{
        if(strcmp(symbol,"+")==0)
            return plussym;
        else if(strcmp(symbol,"-")==0)
             return minussym;
        else if (strcmp(symbol,"*")==0)
            return multsym;
        else if (strcmp(symbol,"/")==0)
            return slashsym;
        else if (strcmp(symbol,"%")==0)
            return oddsym;
        else if (strcmp(symbol,"=")==0)
            return eqsym;
        else if (strcmp(symbol,"<>")==0)
            return neqsym;
        else if (strcmp(symbol,"<")==0)
            return lessym;
        else if (strcmp(symbol,"<=")==0)
            return leqsym;
        else if (strcmp(symbol,">")==0)
            return gtrsym;
        else if (strcmp(symbol,">=")==0)
            return geqsym;
        else if(strcmp(symbol,"(")==0)
            return lparentsym;
        else if(strcmp(symbol,")")==0)
             return rparentsym;
        else if(strcmp(symbol,",")==0)
             return commasym;
        else if(strcmp(symbol,";")==0)
            return semicolonsym;
        else if(strcmp(symbol,".")==0)
            return periodsym;
        else if(strcmp(symbol,":=")==0)
            return becomessym;
        else
            return -1;
    }
}

// Takes in two characters which come from a token from another function and determines if the token is an operator
int is_operator(char first, char second){

    // If the first character's ASCII value is between 0-9 (47-58) it's a positive number, not an operator. Return 0
    if(first > 47 && first < 58)
        return 0;

    // If first characters ASCII value falls between A-Z (64-91) it's a capital letter, not operator. Return 0
    else if (first > 64 && first < 91)
        return 0;

    //If first characters ASCII value falls between a-z (96-123) it's a lowercase letter, not operator. return 0
    else if (first > 96 && first < 123)
        return 0;

    // If first character is a negative (-) and second character is a number, not an operator. Return 0
    else if (first == 45 && second > 47 && second< 58)
        return 0;

    // Otherwise it's an operator. Return 1
    else
        return 1;
}

// Takes in a character array, its length and the predetermined token type value, checks if there is an error
// If no error, calls store_token function
void error_detector(char* symbol, int length, int value){

    // If token is longer than 12 characters, add to list of errors
    if(value == identsym && length > 12)
        error_list(symbol,length,1);

    // If identifier starts with number, add to list of errors
    else if(value == identsym && (symbol[0]>47 && symbol[0]<58))
        error_list(symbol,length,2);

    // If token is a number and greater than 2^16 - 1, add to errors
    else if(value == numbersym && string_to_int(symbol,length)>(int)pow(2,16)-1)
        error_list(symbol,length,3);

    // If token type is -1, it is an invalid token, add to list
    else if(value == -1)
        error_list(symbol,length,4);

    // Otherwise, token is valid and it can be printed with its value using store_token function
    else
        store_token(symbol,length, value);
}

// Takes in a character array of symbols and tries to match symbols to appropriate operators
void process_symbols(char* token,int length){
    int i;

    // Goes through entire token looking for operators
    for(i=0;i<length;i++){

        // If index is at the last position of character array, skip the two character check
        if(i==length-1);

        // Otherwise, check whether i and i+1 form an operator. If so, process operator
        else{
            // Creates a temp character array for checking the two-character token
            char* temp=substring(token,i,i+2);

            // Checks token type using convert function, if not equal to -1, it is a valid operator
            int value=convert(temp,2);
            if(value!=-1){

                // If token is valid, call error_detector for further processing, increment i, and continue.
                error_detector(temp,2,value);
                i++;
                continue;
            }
        }

        // If two character check fails, check character by itself to see if a valid token
        char* temp=substring(token,i,i+1);
        int value=convert(temp,1);
        error_detector(temp,1,value);
    }
}

// Takes in character array and length, converts the string to an integer
int string_to_int(char* symbol, int length){
    int i=0, sum=0, neg=0;

    // If first character is a negative (-), set i and neg to 1
    if(symbol[0]==45)
        i=neg=1;

    while(i<length){

        //As soon as a character isn't a number, return INT_MIN
        if(symbol[i]>57||symbol[i]<48)
            return INT_MIN;

        else
            sum = sum*10 + (symbol[i]-48);
        i++;
    }

    // If positive number, return sum
    if(neg==0)
        return sum;

    // Otherwise, number is negative, return -sum
    return sum*-1;

}

// Takes in a character array, it's length, and token type and stores the string and its corresponding value
void store_token(char* symbol,int length,int value){
    // Check for array out of bounds
    if(currentToken>=size){
        printf("Not enough memory allocated for token arrays - Array out of bounds in store_token function");
        exit(0);
    }
    tokens[currentToken]=malloc(sizeof(char)*length);

    strcpy(tokens[currentToken],symbol);
    tokenType[currentToken]=value;

    currentToken++;
    free(symbol);
}

//Takes in a character array, its length, and the type of error it is, prints out error and terminates the program
void error_list(char* token, int length, int error_type){

    if(error_type==1)
            printf("Error number 28, identifier too long\n");
        else if(error_type==2)
            printf("Error number 27, identifier does not start with a letter \n");
        else if(error_type==3)
            printf("Error number 25, number is too large \n");
        else
            printf("Error number 26, invalid token \n");
    FreeTokenArrays();
    exit(1);
}
// Frees all tokens
void FreeTokenArrays(){

    int i;
    for(i=0;i<currentToken;i++)
        free(tokens[i]);
    free(tokens);
    free(tokenType);
}
