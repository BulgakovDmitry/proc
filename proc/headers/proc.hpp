#ifndef PROC_H
#define PROC_H

#include <stack.hpp>
#include <stdio.h>
#include <string.h>
#include <math.h>                                                  
#include "../../common/headers/commonProcAsm.hpp"

const int    SPECIAL_CASES          = 10;
const int    JUMP_STEP              = 2;
const int    BAD_ARG                = 0;
const int    NUMBER_OF_JMP_TYPE_CMD = 8;

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

struct SPU
{
    double* code;
    size_t codeFileSize;
    int     ip;
    Stack_t stk;
    Stack_t recStk;
    double* registers;
    double* ram;
};

struct PairOfNum {double a, b;};

int  run      (SPU* spu, size_t sizeFile);
SPU* spuInit  (FILE* asmCodeFile);
void spuDelete(SPU* spu);

#endif
