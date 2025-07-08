#include <stdint.h>
#include <myLib.hpp>
#include "../headers/asm.hpp"

static int  ipMove     (char command_recognizer[]);
static int  argInit    (char* argument_recognizer);

static void sintaxError(int num);

static void workWithNum(Asm* asem, char argument_recognizer[], int* shift, int* t_1);
static void workWithRAM(Asm* asem, char argument_recognizer[], int* shift, int t_1);
static void workWithReg(Asm* asem, int* shift, int arg);

static void addLabel   (Asm* asem, char command_recognizer[], size_t len);
static bool searchLabel(Asm* asem, char* buffer, char argument_recognizer[], int* shift, int t_1);

static inline void skipSpaces(const char *buffer, int *shift);

#define WORKING_WITH_THREE_ARGUMENT_COMMAND(constNameCommand)   \
    ((asem->code))[asem->ip] = constNameCommand;                \
    asem->ip++;                                                 \
    shift += t_0; skipSpaces(buffer, &shift);                   \
    char argument_recognizer[LEN_REG] = "";                     \
    int t_1 = 0;                                                \
    sscanf(buffer + shift, "%s%n", argument_recognizer, &t_1);  \
    int arg = argInit(argument_recognizer);                   

#define ONE_ARGUMENT_COMMAND(command, constNameCommand)             \
    else if (strcmp(command_recognizer, command) == 0)              \
    {                                                               \
        ((asem->code))[asem->ip] = constNameCommand;                \
        asem->ip++;                                                 \
        shift += t_0; skipSpaces(buffer, &shift);                   \
        continue;                                                   \
    }

#define JUMP_TYPE_COMMAND(command, constNameCommand)                                  \
    else if (strcmp(command_recognizer, command) == 0)                                \
    {                                                                                 \
        ((asem->code))[asem->ip] = constNameCommand;                                  \
        asem->ip++;                                                                   \
        shift += t_0; skipSpaces(buffer, &shift);                                     \
        char argument_recognizer[MAX_LABEL_SIZE] = "";                                \
        int t_1 = 0;                                                                  \
        sscanf(buffer + shift, "%s%n", argument_recognizer, &t_1);                    \
        if (isdigit(argument_recognizer[0]) != 0)                                     \
        {                                                                             \
            asem->code[asem->ip] = atof(argument_recognizer);                         \
            asem->ip++;                                                               \
            shift += t_1; skipSpaces(buffer, &shift);                                 \
        }                                                                             \
        else                                                                          \
        {                                                                             \
            bool label = searchLabel(asem, buffer, argument_recognizer, &shift, t_1); \
            if (!label)                                                               \
                sintaxError(3);                                                       \
        }                                                                             \
    } 

static inline void skipSpaces(const char* buffer, int* shift)
{
    while (isspace((unsigned char)buffer[*shift]))
        ++(*shift);
}

Asm* asmCtor(FILE* file)
{
    Asm* asem = (Asm*)calloc(1, sizeof(Asm));
    assert(asem);

    asem->asmFileSize = getFileSize(file);

    asem->code = (double*)calloc(asem->asmFileSize + 1, sizeof(double));  
    assert(asem->code);
    asem->code[asem->asmFileSize] = '\0';

    asem->ip = 0;

    asem->lb = (Labels*)calloc(N_LABELS, sizeof(Labels));
    assert(asem->lb);

    asem->labelCounter = 0;

    return asem;
}

void asmDtor(Asm* asem)
{
    assert(asem);

    for (size_t i = 0; i < (size_t)asem->labelCounter; ++i)
        FREE(asem->lb[i].nameLabel);

    if (asem->code)
        FREE(asem->code);
    
    asem->ip          = 0;
    asem->asmFileSize = 0;

    if (asem->lb)
        FREE(asem->lb);

    asem->labelCounter = 0;
    FREE(asem);
}

void asembler(Asm* asem, FILE* assembler)
{
    assert(asem);
    assert(assembler);
    
    char* buffer = readFileToBuffer(assembler);      
    assert(buffer);

    char command_recognizer[10] = "";
    int shift = 0;  

    findLabel(asem, buffer, &shift, command_recognizer); 

    shift = 0;                                          
    asem->ip = 0;
    
    while (buffer[shift] != '\0')
    {   
        int t_0 = 0;
        sscanf(buffer + shift, "%s%n", command_recognizer, &t_0);

        if (strcmp(command_recognizer, "push") == 0)
        {
            WORKING_WITH_THREE_ARGUMENT_COMMAND(COMMAND_PUSH);
            
            if (argument_recognizer[0] == '[')                       
            {                                                        
                workWithRAM(asem, argument_recognizer, &shift, t_1); 
                continue;                                            
            }                                                        
            else if (arg == 0)                                        
            {                                                         
                workWithNum(asem, argument_recognizer, &shift, &t_1); 
                continue;                                             
            }
            else                                
            {                                   
                workWithReg(asem, &shift, arg); 
                continue;                       
            }
        }    
            
        else if (strcmp(command_recognizer, "pop") == 0)
        {
            WORKING_WITH_THREE_ARGUMENT_COMMAND(COMMAND_POP);

            if (argument_recognizer[0] == '[')                       
            {                                                        
                workWithRAM(asem, argument_recognizer, &shift, t_1); 
                continue;                                            
            }                                                        
            else                                
            {                                   
                workWithReg(asem, &shift, arg); 
                continue;                       
            } 
        }

        else if (strncmp(command_recognizer, "hlt", 3) == 0)       
        {                                                          
            ((asem->code))[asem->ip] = COMMAND_HLT;                
            asem->ip++;                                            
            shift += t_0; skipSpaces(buffer, &shift);              
            continue;                                              
        }
        
        JUMP_TYPE_COMMAND("call", COMMAND_CALL)
        JUMP_TYPE_COMMAND("jmp" , COMMAND_JMP )  
        JUMP_TYPE_COMMAND("ja"  , COMMAND_JA  )
        JUMP_TYPE_COMMAND("jb"  , COMMAND_JB  )
        JUMP_TYPE_COMMAND("jae" , COMMAND_JAE )
        JUMP_TYPE_COMMAND("jbe" , COMMAND_JBE )
        JUMP_TYPE_COMMAND("je"  , COMMAND_JE  )
        JUMP_TYPE_COMMAND("jhe" , COMMAND_JHE )

        ONE_ARGUMENT_COMMAND("dump" , COMMAND_DUMP )
        ONE_ARGUMENT_COMMAND("sdump", COMMAND_SDUMP)
        ONE_ARGUMENT_COMMAND("ctg"  , COMMAND_CTG  )
        ONE_ARGUMENT_COMMAND("add"  , COMMAND_ADD  )
        ONE_ARGUMENT_COMMAND("sub"  , COMMAND_SUB  )
        ONE_ARGUMENT_COMMAND("mul"  , COMMAND_MUL  )
        ONE_ARGUMENT_COMMAND("div"  , COMMAND_DIV  )
        ONE_ARGUMENT_COMMAND("sqrt" , COMMAND_SQRT )
        ONE_ARGUMENT_COMMAND("out"  , COMMAND_OUT  ) 
        ONE_ARGUMENT_COMMAND("in"   , COMMAND_IN   )
        ONE_ARGUMENT_COMMAND("sin"  , COMMAND_SIN  )
        ONE_ARGUMENT_COMMAND("cos"  , COMMAND_COS  )
        ONE_ARGUMENT_COMMAND("tg"   , COMMAND_TG   )
        ONE_ARGUMENT_COMMAND("ret"  , COMMAND_RET  )

        else
        { 
            size_t len = strlen(command_recognizer); 
            shift += (int)len; 
            skipSpaces(buffer, &shift);                          
            continue;
        }
    } 

    FREE(buffer);
}

#undef ONE_ARGUMENT_COMMAND
#undef JUMP_TYPE_COMMAND

#undef WORKING_WITH_THREE_ARGUMENT_COMMAND

void findLabel(Asm* asem, char* buffer, int* shift, char command_recognizer[])
{
    assert(asem);
    assert(buffer);
    assert(command_recognizer);
    assert(shift);

    int pos = *shift; 

    while (buffer[pos] != '\0')
    {   
        int read = 0;

        if (sscanf(buffer + pos, "%9s%n", command_recognizer, &read) != 1)
            break; 

        pos += read;   

        while (buffer[pos] == ' ' || buffer[pos] == '\t')
            ++pos;

        size_t len = strlen(command_recognizer);

        asem->ip += ipMove(command_recognizer);

        if (len && command_recognizer[len - 1] == ':')
            addLabel(asem, command_recognizer, len);
    }
    
    *shift = pos; 
}

static void addLabel(Asm* asem, char command_recognizer[], size_t len)
{
    assert(asem);
    assert(command_recognizer);

    command_recognizer[len - 1] = '\0';  // СТИРАНИЕ ДВОЕТОЧИЯ ИЗ НАЗВАНИЯ МЕТКИ
    asem->lb[asem->labelCounter].nameLabel = strdup(command_recognizer);
    asem->lb[asem->labelCounter].ipLabel = asem->ip + 1;
    asem->labelCounter++;
}

static int argInit(char* argument_recognizer)
{
    assert(argument_recognizer);

    int arg = 0;

    for (int i = 0; i < NUMBER_OF_REGISTERS; i++)
    {
        if (strncmp(argument_recognizer, registerNames[i], 2) == 0)
            return i + 1;
    }

    return arg;
}

static void sintaxError(int num)
{
    printf(RED "SINTAX ERROR %d\n" RESET, num);
    abort();
}

static bool searchLabel(Asm* asem, char* buffer, char argument_recognizer[], int* shift, int t_1)
{
    assert(asem);
    assert(buffer);
    assert(argument_recognizer);

    bool metka = false;
    for (int y = 0; y < asem->labelCounter; y++)
    {
        if (strcmp(argument_recognizer, asem->lb[y].nameLabel) == 0)
        {
            asem->code[asem->ip] = (double)asem->lb[y].ipLabel;
            asem->ip++;
            metka = true;
            (*shift) += t_1 + SPACE;
            continue;
        }
    }
    return metka;
} 

static int ipMove(char command_recognizer[])
{
    assert(command_recognizer);
    int ip = 0;
    
    for (int k = 0; k < NUMBER_OF_COOMANDS - 1; k++) // НУЛЕВАЯ КОММАНДА НЕ ВХОДИТ В ЧИСЛО КОРРЕКТНЫХ КОММАНД
    {
        if (strcmp(command_recognizer, commandArr[k].CommandName) == 0) 
        {
            ip += commandArr[k].ipShift;
            continue;
        }
    }
                                                        
    return ip;
}

static void workWithRAM(Asm* asem, char argument_recognizer[], int* shift, int t_1)
{
    assert(asem);
    assert(argument_recognizer);
    (*shift)++;
    asem->code[asem->ip] = ARGTYPE_M;
    asem->ip++;
    argument_recognizer[t_1 - 1] = '\0';
    if (!isalpha(argument_recognizer[1]))
    {
        double argument = atof(argument_recognizer + 1);
        
        if (argument >= RAM_SIZE || argument < 0)
            sintaxError(6);
                                                
        asem->code[asem->ip] = argument;
        asem->ip++;

        (*shift) += t_1;
    }
    else
    {
        asem->code[asem->ip - 1] += ARGTYPE_R;
        int argRam = argInit(argument_recognizer + 1);
        int registr = argRam;

        if (!registr)
            sintaxError(8);

        asem->code[asem->ip] = registr;
        asem->ip++;
        (*shift) += t_1;
    }
}

static void workWithReg(Asm* asem, int* shift, int arg)
{
    assert(asem);
    (*shift) += 3;
    int registr = arg;
    asem->code[asem->ip] = ARGTYPE_R;
    asem->ip++;
    asem->code[asem->ip] = registr;
    asem->ip++;
}

static void workWithNum(Asm* asem, char argument_recognizer[], int* shift, int* t_1)
{
    assert(asem);
    assert(argument_recognizer);

    int argument = 0;
    sscanf(argument_recognizer, "%d%n", &argument, t_1);
    asem->code[asem->ip] = ARGTYPE_I; 
    asem->ip++;
    (*shift) += SPACE;
    asem->code[asem->ip] = argument; 
    asem->ip++;
    (*shift) += (*t_1);
}