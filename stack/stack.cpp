#include "stack.hpp"
#include <myLib.hpp>
#include <math.h>

static void stackDataDump(Stack_t stk);

#ifdef STACK_CANARY_PROTECTION
static void installDataCanaries (Stack_t* stk);
static void removeDataCanaries  (Stack_t* stk);
static void installStackCanaries(Stack_t* stk);
static void removeStackCanaries (Stack_t* stk);
#endif

#ifdef STACK_HASH_PROTECTION
static uint64_t stackDataHashCalc  (const char* start, const char* end);
static uint64_t stackStructHashCalc(const Stack_t* stk); 
#endif

#define VERIFICATION(...)                                                 \
do                                                                        \
{                                                                         \
    if (verifyError != OK)                                                \
    {                                                                     \
        S_DBG(fprintf(stderr, RED "Error: verifyError != OK\n" RESET);)   \
        stackDump(*stk);                                                  \
        stackErrorDump(*stk);                                             \
        __VA_ARGS__                                                       \
    }                                                                     \
} while (0)          

#ifdef STACK_CANARY_PROTECTION

static void installDataCanaries(Stack_t* stk)
{
    S_DBG(ASSERT(stk, "stk = nullptr", stderr);)

    stk->data[0]                 = L_DATA_KANAR;   // INSTALLING A NEW LEFT  CANARY ON DATA
    stk->data[stk->capacity - 1] = R_DATA_KANAR;   // INSTALLING A NEW RIGHT CANARY ON DATA
}

static void removeDataCanaries(Stack_t* stk)
{
    S_DBG(ASSERT(stk, "stk = nullptr", stderr);)

    stk->data[0] = POISON;                         // REMOVING THE OLD LEFT  CANARY (CHANGE TO POISON)
    stk->data[stk->capacity - 1] = POISON;         // REMOVING THE OLD RIGHT CANARY (CHANGE TO POISON)
}
static void installStackCanaries(Stack_t* stk)
{
    S_DBG(ASSERT(stk, "stk = nullptr", stderr);)

    stk->leftStackCanary = L_STACK_KANAR;          // INSTALLING A NEW LEFT  CANARY ON STACK
    stk->rightStackCanary = R_STACK_KANAR;         // INSTALLING A NEW RIGHT CANARY ON STACK
}

static void removeStackCanaries(Stack_t* stk)
{
    S_DBG(ASSERT(stk, "stk = nullptr", stderr);)

    stk->leftStackCanary = 0;                      // REMOVING THE OLD LEFT  CANARY (CHANGE TO 0)
    stk->rightStackCanary = 0;                     // REMOVING THE OLD RIGHT CANARY (CHANGE TO 0)
}
#endif

#ifdef STACK_HASH_PROTECTION
static uint64_t stackDataHashCalc(const char* start, const char* end)
{
    S_DBG(ASSERT(start, "start = nulptr", stderr);)
    S_DBG(ASSERT(end,   "end = nulptr", stderr);)
    S_DBG(bool check = end > start; ASSERT(check, "end > start", stderr);)

    uint64_t hashSum = 5381;
    char* current = const_cast<char*>(start);
    while (current < end)
    {
        hashSum += hashSum * HASH_COEFF + (unsigned char)(*current);
        current++;
    }
    return hashSum;
}

static uint64_t stackStructHashCalc(const Stack_t* stk) 
{
    S_DBG(ASSERT(stk, "stk = nullptr", stderr);)

    Stack_t tmp = *stk;               // local copy
    tmp.dataHashSum  = 0;             // to keep it out of the hash
    tmp.stackHashSum = 0;

    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&tmp);
    const size_t         size  = sizeof(tmp);

    uint64_t hash = 5381u;                     
    for (size_t i = 0; i < size; ++i)
        hash = (hash * HASH_COEFF) + bytes[i];

    return hash;
}
#endif

void stackCtor(Stack_t* stk)
{
    S_DBG(ASSERT(stk, "stk = nullptr", stderr);)
    
    memset(stk, 0, sizeof(*stk)); // Zeroize the structure to prevent garbage from getting into the hash

    S_CAN_PR(installStackCanaries(stk);)
    
    stk->coefCapacity = 2;
    stk->size = 0;
    stk->capacity = START_SIZE;

    stk->data = (StackElem_t*)calloc(stk->capacity, sizeof(StackElem_t));
    if (!stk->data)
    {
        stk->errorStatus = ALLOC_ERROR;
        return;
    }

    S_CAN_PR(installDataCanaries(stk);)

    for (size_t i = 1; i < stk->capacity - 1; i++) 
        stk->data[i] = POISON;

    #ifdef STACK_HASH_PROTECTION
    stk->dataHashSum  = stackDataHashCalc(reinterpret_cast<const char*>(stk->data), reinterpret_cast<const char*>(stk->data + stk->capacity));
    stk->stackHashSum = stackStructHashCalc(stk);
    #endif

    StackError verifyError = (StackError)stackVerify(stk);
    if (verifyError != OK)
    {
        stk->errorStatus |= INIT_HASH_ERROR;
        stackDump(*stk);
        stackErrorDump(*stk);
    }
}

void stackDtor(Stack_t* stk)
{
    S_DBG(ASSERT(stk, "stk = nullptr", stderr);)

    S_CAN_PR(removeDataCanaries(stk);)
    
    FREE(stk->data);
    
    S_CAN_PR(removeStackCanaries(stk);)

    S_HASH_PR(stk->dataHashSum = 0; stk->stackHashSum = 0;)
    memset(stk, 0, sizeof(*stk));
}

StackError stackPush(Stack_t* stk, StackElem_t value)
{
    S_DBG(ASSERT(stk, "stk = nullptr", stderr);)

    if (!stk)
        return POINTER_ERROR;

    StackError verifyError = (StackError)stackVerify(stk);
    VERIFICATION(return verifyError;);

    if (stk->size >= stk->capacity - 2) // CHECKING FOR IMPLEMENTATION
    {    
        S_CAN_PR(removeDataCanaries(stk);)

        size_t newCapacity = stk->capacity * stk->coefCapacity;
        void* newData = (StackElem_t*)realloc(stk->data, newCapacity * sizeof(StackElem_t));
        if (!newData)
        {
            S_CAN_PR(installDataCanaries(stk);)
            
            stk->errorStatus |= ALLOC_ERROR;

            #ifdef STACK_HASH_PROTECTION
                stk->dataHashSum  = stackDataHashCalc(reinterpret_cast<char*>(stk->data), reinterpret_cast<char*>(stk->data + stk->capacity));
                stk->stackHashSum = stackStructHashCalc(stk);
            #endif

            return ALLOC_ERROR;
        }

        stk->data = (StackElem_t*)newData;
        stk->capacity = newCapacity;
        
        for (size_t i = stk->size + 1; i < stk->capacity - 1; i++) // Initialize new memory
            stk->data[i] = POISON;

        S_CAN_PR(installDataCanaries(stk);)
    }    
   
    stk->size++;
    stk->data[stk->size] = value;

    #ifdef STACK_HASH_PROTECTION
    stk->dataHashSum  = stackDataHashCalc(reinterpret_cast<const char*>(stk->data), reinterpret_cast<const char*>(stk->data + stk->capacity));
    stk->stackHashSum = stackStructHashCalc(stk);
    #endif

    verifyError = (StackError)stackVerify(stk); // final check
    VERIFICATION(stk->errorStatus = verifyError; return verifyError;);

    return OK;
}

StackElem_t stackPop(Stack_t* stk)
{
    if (!stk) 
    {
        S_DBG(fprintf(stderr, RED "Error: nullptr passed to stackPop\n" RESET);)
        return POISON;
    }

    StackError verifyError = (StackError)stackVerify(stk);
    VERIFICATION(return POISON;);
   
    if (stk->size == 0) // Checking if stack is empty
    {
        S_DBG(fprintf(stderr, RED "Error: stack is empty\n" RESET);)
        stk->errorStatus |= EMPTY_STACK;
        stackErrorDump(*stk);
        return POISON;
    }
    
    StackElem_t temp = stk->data[stk->size];   
    stk->data[stk->size] = POISON;   
    stk->size--;

    if (stk->size < stk->capacity / (REDUCER_CAPACITY * stk->coefCapacity) && stk->capacity > START_SIZE)
    {
        S_CAN_PR(removeDataCanaries(stk);)
        
        size_t newCapacity = stk->capacity / stk->coefCapacity;

        void* newData = (StackElem_t*)realloc(stk->data, newCapacity * sizeof(StackElem_t));
        if (!newData)
        {
            stk->capacity *= stk->coefCapacity; // save the old capacity (since realloc failed)
            
            stk->errorStatus |= ALLOC_ERROR;
            
            S_CAN_PR(installDataCanaries(stk);)

            #ifdef STACK_HASH_PROTECTION
                stk->dataHashSum  = stackDataHashCalc(reinterpret_cast<char*>(stk->data), reinterpret_cast<char*>(stk->data + stk->capacity));
                stk->stackHashSum = stackStructHashCalc(stk);
            #endif

            return temp;
        }
        else
        {
            stk->data = (StackElem_t*)newData;
            stk->capacity = newCapacity;
            
            S_CAN_PR(installDataCanaries(stk);)
        }
    }

    #ifdef STACK_HASH_PROTECTION
    stk->dataHashSum  = stackDataHashCalc(reinterpret_cast<const char*>(stk->data), reinterpret_cast<const char*>(stk->data + stk->capacity));
    stk->stackHashSum = stackStructHashCalc(stk);
    #endif

    verifyError = (StackError)stackVerify(stk);
    VERIFICATION();

    return temp;
}

StackElem_t stackGet(const Stack_t* stk)
{
    if (!stk) 
    {
        S_DBG(fprintf(stderr, RED "Error: nullptr passed to stackGet\n" RESET);)
        return POISON;
    }

    StackError verifyError = (StackError)stackVerify(const_cast<Stack_t*>(stk));
    VERIFICATION(return POISON;);

    if (stk->size == 0) 
    {
        S_DBG(printf(RED "STACK IS EMPTY\n" RESET);)
        (const_cast<Stack_t*>(stk))->errorStatus |= EMPTY_STACK;
        return POISON;
    }

    #ifdef STACK_HASH_PROTECTION
    uint64_t current_data_hash  = stackDataHashCalc(reinterpret_cast<const char*>(stk->data), reinterpret_cast<const char*>(stk->data + stk->capacity));
    uint64_t current_stack_hash = stackStructHashCalc(stk);
    
    if (current_data_hash != stk->dataHashSum) 
    {
        (const_cast<Stack_t*>(stk))->errorStatus |= DATA_HASH_ERROR;
        S_DBG(fprintf(stderr, RED "DATA HASH MISMATCH in stackGet\n" RESET);)
        return POISON;
    }
    
    if (current_stack_hash != stk->stackHashSum) 
    {
        (const_cast<Stack_t*>(stk))->errorStatus |= STACK_HASH_ERROR;
        S_DBG(fprintf(stderr, RED "STRUCT HASH MISMATCH in stackGet\n" RESET);)
        return POISON;
    }
    #endif

    return stk->data[stk->size];
}

#undef VERIFICATION

static void stackDataDump(Stack_t stk)
{
    printf("%s{%s\n", GREEN, RESET);

    for (size_t i = 0; i < stk.capacity; i++)
        printf("  %s[%s%3zu%s]   = %s%3lg%s\n", GREEN, MANG, i, GREEN, RED, stk.data[i], RESET);
    
    printf("%s}%s\n", GREEN, RESET);
    
    printf("%sstk.data%s[%s0%s]            = %s%lg %s[ %shex%s %X%s ]%s", BLUE, GREEN, MANG, GREEN, RED, stk.data[0], MANG, BLUE, RED, (unsigned int)stk.data[0], MANG, RESET);
    S_CAN_PR(printf(" %s[%s true hex%s %X %s]%s", MANG, BLUE, RED, (unsigned int)L_DATA_KANAR, MANG, RESET);)
    putchar('\n');
    printf("%sstk.data%s[%scapacity - 1%s] = %s%lg %s[ %shex%s %X%s ]%s", BLUE, GREEN, MANG, GREEN, RED, stk.data[stk.capacity - 1], MANG, BLUE, RED, (unsigned int)stk.data[stk.capacity - 1], MANG, RESET);
    S_CAN_PR(printf(" %s[%s true hex%s %X %s]%s", MANG, BLUE, RED, (unsigned int)R_DATA_KANAR, MANG, RESET);)
    putchar('\n');
}

void stackDump(Stack_t stk)
{
    printf("%s___stackDump___~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s\n", RED, RESET);
    #ifdef STACK_CANARY_PROTECTION
    printf("%s{%sL_STACK_KANAR %s= %s%X%s", GREEN, BLUE, GREEN, RED, (unsigned int)stk.leftStackCanary, RESET);
    printf("%s, %sR_STACK_KANAR %s= %s%X%s}%s\n", GREEN, BLUE, GREEN, RED, (unsigned int)stk.rightStackCanary, GREEN, RESET);
    printf("%s{%sL_DATA_KANAR %s = %s%X%s, %s", GREEN, BLUE, GREEN, RED, (unsigned int)stk.data[0], GREEN, RESET);
    printf("%sR_DATA_KANAR %s = %s%X%s}%s\n", BLUE, GREEN, RED, (unsigned int)stk.data[stk.capacity - 1], GREEN, RESET);
    #endif

    printf("%scapasity %s= %s%zu%s  ", BLUE, GREEN, RED, stk.capacity, RESET);
    printf("%ssize %s= %s%zu%s\n", BLUE, GREEN, RED, stk.size, RESET);
    printf("%sdata %s[%s%p%s]%s\n", CEAN, GREEN, MANG, stk.data, GREEN, RESET);

    stackDataDump(stk);
    printf("%s~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s\n", RED, RESET);
}


uint64_t stackVerify(Stack_t* stk)
{
    S_DBG(ASSERT(stk, "stk = nullptr", stderr);)
    
    if (!stk)
        return POINTER_ERROR;
    
    uint64_t errors = OK;

    if (stk->size > stk->capacity - 2)  // -2 for canaries
        errors |= SIZE_ERROR;
    
    #ifdef STACK_CANARY_PROTECTION
    if (!doubleCmp(stk->leftStackCanary, L_STACK_KANAR))
        errors |= LEFT_STACK_CANARY_DIED;
        
    if (!doubleCmp(stk->rightStackCanary, R_STACK_KANAR))
        errors |= RIGHT_STACK_CANARY_DIED;
    
    if (!stk->data)
    {
        if (stk -> capacity == 0)
            errors |= POINTER_ERROR;
    }
    else 
    {
        if (!doubleCmp(stk->data[0], L_DATA_KANAR))                  // check left data canary
            errors |= LEFT_DATA_CANARY_DIED;                        // 
            
        if (!doubleCmp(stk->data[stk->capacity - 1], R_DATA_KANAR))  // check right data canary
            errors |= RIGHT_DATA_CANARY_DIED;                       // 
    }
    #endif

    #ifdef STACK_HASH_PROTECTION
    if (stk->data && stk->capacity > 0) // check data hash
    {
        uint64_t currentDataHash = stackDataHashCalc(reinterpret_cast<const char*>(stk->data), reinterpret_cast<const char*>(stk->data + stk->capacity));
        if (currentDataHash != stk->dataHashSum)
            errors |= DATA_HASH_ERROR;
    }

    
    uint64_t currentStackHash = stackStructHashCalc(stk);
    if (currentStackHash != stk->stackHashSum) // check stack hash
        errors |= STACK_HASH_ERROR;
    #endif
        
    stk->errorStatus = errors;
    return (StackError)errors;
}
    
static const char* stackErrors[NUMBER_OF_ERRORS] = {
                                                    "POINTER_ERROR",
                                                    "ALLOC_ERROR",
                                                    "SIZE_ERROR",
                                                    "LEFT_STACK_CANARY_DIED",
                                                    "RIGHT_STACK_CANARY_DIED",
                                                    "LEFT_DATA_CANARY_DIED",
                                                    "RIGHT_DATA_CANARY_DIED",
                                                    "EMPTY_STACK",
                                                    "STACK_HASH_ERROR",
                                                    "DATA_HASH_ERROR",
                                                    "INIT_HASH_ERROR",   
                                                   };
StackError stackErrorDump(Stack_t stk)
{
    printf("%s___stackErrorDump___~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s\n", RED, RESET);
    for (size_t i = 0; i < NUMBER_OF_ERRORS; i++)
    {
        if (stk.errorStatus & (1 << i))
            fprintf(stderr, RED"error: code %zu ( %s )\n"RESET, i + 1, stackErrors[i]);
    }
    return OK;
}
