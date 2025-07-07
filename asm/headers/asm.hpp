#ifndef PROC_ASSM_H
#define PROC_ASSM_H

#include <myLib.hpp>
#include "../../common/commonProcAsm.hpp"
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

static const int SPACE          = 1 ;
static const int LEN_REG        = 8 ;
static const int MAX_LABEL_SIZE = 40;
static const char* const ASSEMBLER_FILE_NAME = "assemblerFile.txt";

struct Labels
{
    char* nameLabel;
    size_t       ipLabel;
};

struct Asm
{
    size_t asmFileSize;
    double* code;
    int ip;
    Labels* lb;
    int labelCounter;
};

struct IP_MOVE        
{
    const char* const CommandName; 
    int ipShift;
    ComputingCommands constNameCommand;
};

static const int NUMBER_OF_JMP_TYPE_COMMAND     = 8;
static const int NUMBER_OF_ONE_ARGUMENT_COMMAND = 14;

static const IP_MOVE commandArr[NUMBER_OF_COOMANDS] = {
                                                        {"ja", 2, COMMAND_JA}, {"jb", 2, COMMAND_JB}, {"jae", 2, COMMAND_JAE}, 
                                                        {"jbe" , 2, COMMAND_JBE}, {"je", 2, COMMAND_JE}, 
                                                        {"jhe", 2, COMMAND_JHE}, {"call", 2, COMMAND_CALL}, {"jmp", 2, COMMAND_JMP},
                                                        {"ctg" , 1, COMMAND_CTG}, {"dump", 1, COMMAND_DUMP}, {"sdump", 1, COMMAND_SDUMP}, 
                                                        {"add" , 1, COMMAND_ADD}, {"sub", 1, COMMAND_SUB}, {"mul", 1, COMMAND_MUL}, 
                                                        {"div" , 1, COMMAND_DIV}, {"sqrt", 1, COMMAND_SQRT}, {"out"  , 1, COMMAND_OUT},
                                                        {"in"  , 1, COMMAND_IN}, {"sin", 1, COMMAND_SIN}, {"cos", 1, COMMAND_COS}, 
                                                        {"ret" , 1, COMMAND_RET}, {"tg"  , 1, COMMAND_TG}, 
                                                        {"hlt" , 1, COMMAND_HLT}, 
                                                        {"push", 3, COMMAND_PUSH}, {"pop", 3, COMMAND_POP},
                                                      };

const char* const registerNames[NUMBER_OF_REGISTERS] = {"ax", "bx", "cx", "dx", "ex", "fx", "gx", "hx"};

void asembler(Asm* asem, FILE* assembler);
Asm* asmCtor(FILE* file);
void asmDtor(Asm* asem);

void  findLabel(Asm* asem, char* buffer, int* i, char command_recognizer[]); // первичный прогон по коду для определения меток

#endif