#ifndef PROC_ASSM_H
#define PROC_ASSM_H\

#include "proc.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define FCLOSE(ptr_) \
    do {fclose(ptr_); ptr_ = NULL;} while(false)

struct CMD_ARRAY      {const char* cmd_array;};
struct ARG_RECOGNIZER {int a, b, c, d, e, f, g, h;};

size_t asembler(SPU* spu);

size_t*        getFileSize     (const char* name_file);
char*          readFileToBuffer(FILE* file, size_t size_file);
int            searchRegister  (ARG_RECOGNIZER arg);
void           defineLabel     (SPU* spu, char command_recognizer[]);
ARG_RECOGNIZER argInit         (char* argument_recognizer);
bool           checkArg        (ARG_RECOGNIZER arg);
bool           searchLabel     (SPU* spu, char* buffer, char argument_recognizer[], int* i, int t_1);
void           createComment   (int* i, char command_recognizer[]);
void           createLabel     (SPU* spu, int* i, char command_recognizer[]);
void           sintaxError     (int num);
void           addLabel        (SPU* spu, char command_recognizer[], size_t len);


    
#endif