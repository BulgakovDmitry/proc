#ifndef STACK_H
#define STACK_H

typedef double StackElem_t;
typedef double Canary_t;
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

    #define DEBUG

#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define MANG    "\033[1;35m"
#define WHITE   "\033[1;37m"
#define GREEN   "\033[1;32m"
#define BLUE    "\033[1;34m"
#define CEAN    "\033[1;36m"
#define YELLOW  "\033[1;33m"

#define FREE(ptr_) \
        do { free(ptr_); ptr_ = NULL; } while (false)

#define INFO() \
        Info(__FILE__, __LINE__, __func__)

#define STACK_ASSERT(stk) \
        StackAssertFunk((stk), __FILE__, __func__, __LINE__)

#define STACK_CTOR(stk) \
        do { StackCtor((stk)); INFO(); } while (false)
#ifdef DEBUG
        #define ON_DEBUG(code) code
    #else
        #define ON_DEBUG(code)
    #endif    

struct Info_locate
{
    const char*  file;
    const char*  func;
    size_t       line;
};

struct Stack_t
{
    Canary_t     L_STACK_KANAR;
    Info_locate  info_Stack_t;    
    size_t       koef_capacity;
    int          error_status;
    ///////////////////////////////////////
    StackElem_t* data;
    size_t       size;
    size_t       capacity;
    ///////////////////////////////////////
    Canary_t     R_STACK_KANAR;
};

const size_t      START_SIZE    = 16;
const StackElem_t POISON        = -666;
const Canary_t    L_DATA_KANAR  = 0xEDAA;
const Canary_t    R_DATA_KANAR  = 0xF00D;
const Canary_t    L_STACK_KANAR = 0xBEDA;
const Canary_t    R_STACK_KANAR = 0x0DED;

enum ErrorCodes
{
    InvalidStructurePointer = -1,
    InvalidStackPointer     = -2,
    StackOK                 = 0,
    /////////////////////////////////////////
    UndefinedStackError     = -4,
    IncorrectStackSize      = 8,
    SizeExceededCapacity    = 16,
    LeftAttackOnStructure   = 32,
    RightAttackOnStructure  = 64,
    LeftAttackOnStack       = 128,
    RightAttackOnStack      = 256,
    //IncorrectHashOfStack  = 512,
    //IncorrectHashOfStruct = 1024,
};

////////////////////___СОЗДАНИЕ И ОЧИСТКА СТЕКА___/////////////////////////////////////////////////////////////////////////////////////////////////
int StackCtor (Stack_t* stk);   //  СОЗДАНИЕ И 
int StackDtor (Stack_t* stk);   //  ОЧИСТКА СТЕКА

////////////////////__ДОБАВЛЕНИЯ ИЛИ УДАЛЕНИЕ ЭЛЕМЕНТОВ, РАБОТА СО СТЕКОМ___///////////////////////////////////////////////////////////////////////
void        StackPush(Stack_t* stk, StackElem_t elem );
StackElem_t StackPop (Stack_t* stk                   );

////////////////////___ВСЕВОЗМОЖНЫЕ ПРОВЕРКИ СТЕКА НА ОШИБКИ, АТАКУ И КОРРЕКТНОСТЬ АДРЕСОВ/////////////////////////////////////////////////////////
void        StackCheckAssertError(Stack_t* stk                                                 );
void        StackAssertFunk      (Stack_t* stk, const char* FILE, const char* FUNC, size_t LINE);
int         StackOk              (Stack_t* stk                                                 );
void        StackError           (Stack_t* stk                                                 );
int         StackCheckError      (Stack_t* stk                                                 );
void        StackAssertError     (Stack_t* stk                                                 );
int         StackPrintError      (int      error                                               );
Info_locate Info                 (const char* FILE, size_t LINE, const char* FUNC              );

////////////////////___РАСПЕЧАТКА СОСТОЯНИЯ СТЕКА___///////////////////////////////////////////////////////////////////////////////////////////////
void        StackDump (Stack_t*    stk,  Info_locate info_l           );
void        StackPrint(Stack_t*    stk                                );

#endif