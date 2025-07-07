#ifndef STACK_MAIN_H
#define STACK_MAIN_H

typedef double StackElem_t;
typedef double Canary_t;

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include "configFile.hpp"

#ifdef STACK_DEBUG
    #define S_DBG(...) __VA_ARGS__
#else
    #define S_DBG(...)
#endif

#ifdef STACK_CANARY_PROTECTION
    #define S_CAN_PR(...) __VA_ARGS__
#else
    #define S_CAN_PR(...)
#endif

#ifdef STACK_HASH_PROTECTION
    #define S_HASH_PR(...) __VA_ARGS__
#else
    #define S_HASH_PR(...)
#endif

struct Stack_t
{
    S_CAN_PR(Canary_t leftStackCanary;)

    size_t   coefCapacity;
    uint64_t errorStatus;

    StackElem_t* data;
    size_t       size;
    size_t       capacity;

    #ifdef STACK_HASH_PROTECTION
    uint64_t dataHashSum;
    uint64_t stackHashSum;
    #endif

    S_CAN_PR(Canary_t rightStackCanary;)
};

enum StackError
{
    OK                      = 0,
    POINTER_ERROR           = 1 << 0,
    ALLOC_ERROR             = 1 << 1,
    SIZE_ERROR              = 1 << 2,
    LEFT_STACK_CANARY_DIED  = 1 << 3,
    RIGHT_STACK_CANARY_DIED = 1 << 4,
    LEFT_DATA_CANARY_DIED   = 1 << 5,
    RIGHT_DATA_CANARY_DIED  = 1 << 6,
    EMPTY_STACK             = 1 << 7,
    STACK_HASH_ERROR        = 1 << 8,
    DATA_HASH_ERROR         = 1 << 9,
    INIT_HASH_ERROR         = 1 << 10,   
    NUMBER_OF_ERRORS
};

const size_t START_SIZE       = 16;
const StackElem_t POISON      = -666;
const size_t REDUCER_CAPACITY = 2;
const uint64_t HASH_COEFF     = 33;

const Canary_t L_DATA_KANAR  = 0xEDAA;
const Canary_t R_DATA_KANAR  = 0xF00D;
const Canary_t L_STACK_KANAR = 0xBEDA;
const Canary_t R_STACK_KANAR = 0x0DED;

void stackCtor(Stack_t* stk);
void stackDtor(Stack_t* stk);

StackError  stackPush(Stack_t* stk, StackElem_t value);
StackElem_t stackPop(Stack_t* stk);
StackElem_t stackGet(const Stack_t* stk);

uint64_t stackVerify(Stack_t* stk);

void stackDump(Stack_t stk);
StackError stackErrorDump(Stack_t stk);

#endif
