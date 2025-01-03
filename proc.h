#ifndef PROC_H
#define PROC_H
#include "Stack.h"
#include <stdio.h>
#include <string.h>
#include <math.h>                                                  


const int    N_LABELS       = 10;
const int    MAX_LABEL_SIZE = 40;
const int    RAM_SIZE       = 30;
const int    JUMP_STEP      = 2;
const int    BAD_ARG        = 0;
const double NUMBER_OF_REGISTERS = 8;

enum ComputingCommands
{ 
    COMMAND_NULL  = 0,
    COMMAND_PUSH  = 1,
    COMMAND_POP   = 2,
    COMMAND_JMP   = 3,
    COMMAND_JA    = 4,
    COMMAND_JB    = 5,
    COMMAND_JAE   = 6,
    COMMAND_JBE   = 7,
    COMMAND_JE    = 8,
    COMMAND_JHE   = 9,
    COMMAND_HLT   = 100,
    COMMAND_DUMP  = 101,
    COMMAND_SDUMP = 102,
    COMMAND_ADD   = 103,
    COMMAND_SUB   = 104,
    COMMAND_MUL   = 105,
    COMMAND_DIV   = 106,
    COMMAND_SQRT  = 107,
    COMMAND_OUT   = 108,
    COMMAND_IN    = 109,
    COMMAND_SIN   = 110,
    COMMAND_COS   = 111,
    COMMAND_TG    = 112,
    COMMAND_CTG   = 113,
    COMMAND_CALL  = 150,
    COMMAND_RET   = 151,
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
