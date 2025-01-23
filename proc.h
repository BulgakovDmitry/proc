#ifndef PROC_H
#define PROC_H
#include "Stack.h"
#include <stdio.h>
#include <string.h>
#include <math.h>                                                  


const int    N_LABELS       = 10;
const int    MAX_LABEL_SIZE = 40;
const int    RAM_SIZE       = 20;
const int    SPECIAL_CASES  = 10;
const int    JUMP_STEP      = 2;
const int    BAD_ARG        = 0;
const int    NUMBER_OF_REGISTERS = 8;

enum ComputingCommands
{ 
    COMMAND_NULL  = 0,
    COMMAND_PUSH , 
    COMMAND_POP  ,
    COMMAND_JMP  ,
    COMMAND_JA   ,
    COMMAND_JB   ,
    COMMAND_JAE  ,
    COMMAND_JBE  ,
    COMMAND_JE   ,
    COMMAND_JHE  ,
    COMMAND_HLT  ,
    COMMAND_DUMP ,
    COMMAND_SDUMP,
    COMMAND_ADD  ,
    COMMAND_SUB  ,
    COMMAND_MUL  ,
    COMMAND_DIV  ,
    COMMAND_SQRT ,
    COMMAND_OUT  ,
    COMMAND_IN   ,
    COMMAND_SIN  , 
    COMMAND_COS  , 
    COMMAND_TG   , 
    COMMAND_CTG  , 
    COMMAND_CALL ,
    COMMAND_RET  ,
    NUMBER_OF_COOMANDS
    };
 
enum ArgTypes 
{
    ARGTYPE_I = 1,
    ARGTYPE_R = 2, 
    ARGTYPE_M = 4,
};

enum Registers 
{
    REGISTER_NULL = 0,
    REGISTER_ax   = 1, 
    REGISTER_bx   = 2, 
    REGISTER_cx   = 3, 
    REGISTER_dx   = 4, 
    REGISTER_ex   = 5, 
    REGISTER_fx   = 6, 
    REGISTER_gx   = 7, 
    REGISTER_hx   = 8, 
};

struct Labels
{
    const char* nameLabel;
    size_t       ipLabel;
};

struct SPU
{
    double* code;
    int     ip;
    Stack_t stk;
    Stack_t recStk;
    double* registers;
    double* ram;
    Labels* lb;
    int     labelCounter;
};

struct PairOfNum {double a, b;};

int  run        (SPU* spu, size_t size_file);

int  getArgPuch (SPU* spu);
int  getRegister(int argument);
int  getArgPop  (SPU* spu);
SPU* spuInit    ();
void pDump      (SPU spu);
void spuDelete  (SPU* spu);
void printDebug (int ind);
void jump       (SPU* spu);

PairOfNum getPairOfNumbersFromStack(SPU* spu);

#endif
