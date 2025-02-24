#ifndef COMMON_PROC_ASM_H
#define COMMON_PROC_ASM_H

enum ComputingCommands
{ 
    COMMAND_NULL  = 0,
    COMMAND_PUSH  = 1/*0x757073*/, 
    COMMAND_POP   = 2,
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

const int N_LABELS = 10;
const int NUMBER_OF_REGISTERS = 8;
const int RAM_SIZE = 20;
const char* const ASM_CODE_FILE = "asmCodeFile.txt";

#endif