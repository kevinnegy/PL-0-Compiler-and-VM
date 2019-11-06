// Compiler Builder 25
// Elizabeth Maspoch
// Hemali Mistry
// Kevin Negy
// Courtney Tennant

#include <stdio.h>
#include <stdlib.h>


struct instruction{
int op;   // opcode
int  l;   // L
int  m;   // M
} ;


void printstatus();
int base( int level, int b );
void LIT(struct instruction ir);
void OPR(struct instruction ir);
void LOD(struct instruction ir);
void STO(struct instruction ir);
void CAL(struct instruction ir);
void INC(struct instruction ir);
void JMP(struct instruction ir);
void JPC(struct instruction ir);
int SIO(struct instruction ir);


#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXT_LEVELS 3


// Declare sp, bp, pc and the stack array
int sp = 0;
int bp = 1;
int pc = 0;
int stack[MAX_STACK_HEIGHT]={0};

int main(int argc, char * argv[]){
    
    if(argc != 2){
        printf("./vm <input>\n");
        return -1;
    }

    printf("PL/0 code:\n\n");
    // Create the ir instruction register and an array of instructions
    struct instruction ir;
    struct instruction *code=malloc(sizeof(struct instruction)*MAX_CODE_LENGTH);
    int i=0;

    // Open command line argument file to get instructions
    FILE *fp;
    fp=fopen(argv[1],"r");

    // Until the end of file is reached, scan in instructions into code array
    while(!feof(fp)){
        fscanf(fp,"%d %d %d",&code[i].op, &code[i].l,&code[i].m);

        // Print out appropriate instructions in order and including relevant information
        switch(code[i].op){
            case 1:

                printf("%3d  LIT       %3d  \n",i, code[i].m);

                break;
              case 2:{
                switch (code[i].m){
                    case 0:{
                        printf("%3d  RET            \n", i);
                        break;
                    }
                    case 1:{
                        printf("%3d  NEG            \n", i);
                        break;
                    }
                    case 2:{
                        printf("%3d  ADD            \n", i);
                        break;
                    }
                    case 3:{
                        printf("%3d  SUB            \n", i);
                        break;
                    }
                    case 4:{
                        printf("%3d  MUL            \n", i);
                        break;
                    }
                    case 5:{
                        printf("%3d  DIV            \n", i);
                        break;
                    }
                    case 6:{
                        printf("%3d  ODD            \n", i);
                        break;
                    }
                    case 7:{
                        printf("%3d  MOD            \n", i);
                        break;
                    }
                    case 8:{
                        printf("%3d  EQL            \n", i);
                        break;
                    }
                    case 9:{
                        printf("%3d  NEQ            \n", i);
                        break;
                    }
                    case 10:{
                        printf("%3d  LSS            \n", i);
                        break;
                    }
                    case 11:{
                        printf("%3d  LEQ            \n", i);
                        break;
                    }
                    case 12:{
                        printf("%3d  GTR            \n", i);
                        break;
                    }
                    case 13:{
                        printf("%3d  GEQ            \n", i);
                        break;
                    }
                }
                break;
            }

            case 3:
                printf("%3d  LOD    %d  %3d  \n",i, code[i].l, code[i].m);
                break;

            case 4:
                printf("%3d  STO    %d  %3d  \n", i,code[i].l, code[i].m);
                break;

            case 5:
                printf("%3d  CAL    %d  %3d  \n",i,code[i].l, code[i].m);
                break;

            case 6:
                printf("%3d  INC       %3d  \n",i, code[i].m);
                break;

            case 7:
                printf("%3d  JMP       %3d  \n",i, code[i].m);
                break;

            case 8:
                printf("%3d  JPC       %3d  \n", i, code[i].m);
                break;

            case 9:{
                if (code[i].m==0){
                    printf("%3d  OUT       %3d  \n", i, code[i].m);
                }

                else if (code[i].m==1){
                    printf("%3d  INP       %3d  \n", i, code[i].m);
                }

                else{
                    printf("%3d  HLT            \n", i);
                }

                break;
            }
        }
        i++;
    }

    printf("\nExecution:\n");
    printf("                      pc   bp   sp   stack\n");
    printf("                      %2d   %2d   %2d   ", pc,bp,sp);

    while(1){
        // Fetch Cycle
        ir=code[pc];
        pc++;
        printf("\n");

        // Execution Cycle
        if(ir.op==1)
            LIT(ir);
        else if(ir.op==2)
            OPR(ir);
        else if(ir.op==3)
            LOD(ir);
        else if(ir.op==4)
            STO(ir);
        else if (ir.op==5){
            CAL(ir);
            //getchar();
            }
        else if(ir.op==6)
            INC(ir);
        else if (ir.op==7)
            JMP(ir);
        else if(ir.op==8){
            JPC(ir);
            }
        else if(ir.op==9){
            // If SIO returns 1, halt is executed. Break out of switch
            if(SIO(ir)==1)
                break;
        }
    }
    printf("\n");
    return 0;
}

// Calculates the bp L levels up from the current bp
int base( int level, int b ) {
    while (level > 0) {
        b = stack[ b + 1 ];
        level--;
    }

    // Stack array does not use 0 index, if calculation results in b==0, increment to 1
    if(b==0)
      b++;
    return b;
}

// Pushes a value(ir.m) onto the stack
void LIT(struct instruction ir){
    printf("%3d  LIT       %3d   ", pc-1, ir.m);
    sp+=1;
    stack[sp]=ir.m;
    printstatus();
}

// Gets value at offset m in frame l levels down and pushes onto stack
void LOD(struct instruction ir){
    //printf("%d\n", base(ir.l,bp)+ir.m);
    printf("%3d  LOD   %2d  %3d   ", pc-1,ir.l,ir.m);
    sp+=1;
    stack[sp]=stack[base(ir.l, bp)+ir.m];
    printstatus();

}

// Pops stack and stores value at offset M, L levels down the stack
void STO(struct instruction ir){
    printf("%3d  STO   %2d  %3d   ", pc-1,ir.l,ir.m);
    stack[base(ir.l, bp)+ir.m]=stack[sp];
    sp-=1;
    printstatus();
}

// Creates a new stack frame (essentially a new sub function)
void CAL(struct instruction ir){
    stack[sp+1]=0;
    stack[sp+2]=base(ir.l,bp);
    stack[sp+3]=bp;
    stack[sp+4]=pc;
    printf("%3d  CAL   %2d  ", pc-1,ir.l);
    pc=ir.m;
    bp=sp+1;
    printf("%3d   ",pc);
    printstatus();

}

// Allocate M local variables on the stack
void INC(struct instruction ir){
    printf("%3d  INC       %3d   ", pc-1,ir.m);
    sp+=ir.m;
    printstatus();
}

int SIO(struct instruction ir){
    // OUT function pops stack and prints out value
    if (ir.m==0){
        printf("%3d  OUT             ", pc-1);
        sp--;
        printstatus();
        printf("\n%d",stack[sp+1]);
        return 0;
    }
    // INP reads input from user and pushes onto stack
    if (ir.m==1){
        printf("%3d  INP             ", pc-1);
        sp++;
        scanf("%d",&stack[sp]);
        printstatus();
        printf("\nread %d from input",stack[sp]);
        return 0;
    }
    // HALT ends the program
    if(ir.m==2){
        printf("%3d  HLT             ", pc-1);
        printstatus();
        return 1;
    }
}

// Changes PC to ir.m so that next fetch cycle "jumps" to desired instruction
void JMP(struct instruction ir){
    printf("%3d  JMP       %3d   ", pc-1,ir.m);
    pc=ir.m;
    printstatus();

}

// Pops stack and sets pc to ir.m if the value popped==0
void JPC(struct instruction ir) {
    printf("%3d  JPC       %3d   ", pc-1, ir.m);
    if (stack [sp] == 0)
        pc = ir.m;
    sp--;
    printstatus();
}

// Prints out pc, bp,sp, and the current stack
void printstatus(){
    printf("%3d   %2d   %2d   ", pc,bp,sp);
    int i,j;
    if(sp!=0){
        // ignore sp[0], start at 1
        for(i=1;i<=sp;i++){
            printf("%d ",stack[i]);
            int tempbp=1,level=0;
            while(level<=MAX_LEXT_LEVELS){
                tempbp=base(level,bp);
                if(tempbp==i+1 && i!=sp ){
                    printf("| ");
                    break;
                }
                level++;
            }
        }
    }
}

// All arithmetic functions
void OPR(struct instruction ir){

    switch(ir.m){

    // RET function used for returning control to parent function
    case 0:
      printf("%3d  RET             ", pc-1);
      sp = bp - 1;
      pc = stack[sp+4];
      bp = stack[sp+3];
      break;

    // NEG function multiplies the top stack value by -1
    case 1:
      printf("%3d  NEG             ", pc-1);
      stack[sp] = -stack[sp];
      break;

    // Adds top two values in the stack and places result on stack
    case 2:
      printf("%3d  ADD             ", pc-1);
      sp=sp-1;
      stack[sp] = stack[sp] + stack[sp+1];
      break;

    // Subtract function
    case 3:
      printf("%3d  SUB             ", pc-1);
      sp=sp-1;
      stack[sp] = stack[sp] - stack[sp+1];
      break;

    // Multiplication function
    case 4:
      printf("%3d  MUL             ", pc-1);
      sp=sp-1;
      stack[sp] = stack[sp] * stack[sp+1];
      break;

    // Divide function
    case 5:
      printf("%3d  DIV             ", pc-1);
      sp=sp-1;
      stack[sp] = stack[sp] / stack[sp+1];
      break;

    // Pushes 1 onto stack if top of stack is an odd number, 0 if even
    case 6:
      printf("%3d  ODD             ", pc-1);
      stack[sp] = stack[sp] % 2;
      break;

    // Mods top two stack values and places result on stack
    case 7:
      printf("%3d  MOD             ", pc-1);
      sp=sp-1;
      stack[sp] = stack[sp] % stack[(sp)+1];
      break;

    // Checks if top two stack values are equal. 1 if equal, 0 if not equal
    case 8:
      printf("%3d  EQL             ", pc-1);
      sp=sp-1;
      stack[sp] = stack[sp] == stack[sp+1];
      break;

    // NEQ function checks that top two values are not equal
    case 9:
      printf("%3d  NEQ             ", pc-1);
      sp=sp-1;
      stack[sp] = stack[sp] != stack[sp+1];
      break;

    // If penultimate value on stack less than top of stack, set top to 1, else 0
    case 10:
      printf("%3d  LSS             ", pc-1);
      sp=sp-1;
      stack[sp] = stack[sp] < stack[sp+1];
      break;

    // If penultimate value on stack less than or equal top of stack, set top to 1, else 0
    case 11:
      printf("%3d  LEQ             ", pc-1);
      sp=sp-1;
      stack[sp] = stack[sp] <= stack[sp+1];
      break;

    // If penultimate value on stack greater than top of stack, set top to 1, else 0
    case 12:
      printf("%3d  GTR             ", pc-1);
      sp=sp-1;
      stack[sp] = stack[sp] > stack[sp+1];
      break;

    // If penultimate value on stack greater than or equal top of stack, set top to 1, else 0
    case 13:
      printf("%3d  GEQ             ", pc-1);
      sp=sp-1;
      stack[sp] = stack[sp] >= stack[sp+1];
      break;
    }
    printstatus();
}
