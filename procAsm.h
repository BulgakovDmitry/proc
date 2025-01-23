#ifndef PROC_ASSM_H
#define PROC_ASSM_H

#include "proc.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define FCLOSE(ptr_) \
    do {fclose(ptr_); ptr_ = NULL;} while(false)

const int SPACE   = 1;
const int LEN_REG = 3;

struct CMD_ARRAY      {const char* cmd_array;};

struct IP_MOVE        
{
    const char* const CommandName; 
    int ipShift;
};

const IP_MOVE commandArr[NUMBER_OF_COOMANDS] = {
                                                    {"push", 3}, {"pop", 3}, {"hlt", 1}, {"dump", 1}, {"sdump", 1}, {"add", 1}, 
                                                    {"sub" , 1}, {"mul", 1}, {"div", 1}, {"sqrt", 1}, {"out"  , 1}, {"in" , 1}, 
                                                    {"sin" , 1}, {"cos", 1}, {"ret", 1}, {"jmp" , 2}, {"ja"   , 2}, {"jb" , 2}, 
                                                    {"jae" , 2}, {"jbe", 2}, {"je" , 2}, {"jhe" , 2}, {"call" , 2}, {"tg" , 1},
                                                    {"ctg" , 1}
                                               };

const char* const registerNames[NUMBER_OF_REGISTERS] = {"ax", "bx", "cx", "dx", "ex", "fx", "gx", "hx"};

size_t asembler(SPU* spu);

size_t* getFileSize     (const char* name_file);
char*   readFileToBuffer(FILE* file, size_t size_file);
int     ipMove          (char command_recognizer[]);
void    defineLabel     (SPU* spu, char command_recognizer[]);
int     argInit         (char* argument_recognizer);
bool    searchLabel     (SPU* spu, char* buffer, char argument_recognizer[], int* i, int t_1);
void    createLabel     (SPU* spu, int* i, char command_recognizer[]);
void    sintaxError     (int num);
void    addLabel        (SPU* spu, char command_recognizer[], size_t len);
void    findLabel       (SPU* spu, char* buffer, int* i, char command_recognizer[]);
void    workWithRAM     (SPU* spu, char argument_recognizer[], int* i, int t_1);
void    workWithReg     (SPU* spu, int* i, int arg);
void    workWithNum     (SPU* spu, char argument_recognizer[], int* i, int* t_1);






    
#endif