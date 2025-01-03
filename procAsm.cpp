#include "procAsm.h"

#define ONE_ARGUMENT_COMMAND(command, constNameCommand)                          \
    else if (strcmp(command_recognizer, command) == 0)                           \
    {                                                                            \
        spu->code[spu->ip] = constNameCommand;                                   \
        spu->ip++;                                                               \
        i += t_0 + SPACE;                                                        \
        continue;                                                                \
    }

#define JUMP_TYPE_COMMAND(command, constNameCommand)                             \
    else if (strcmp(command_recognizer, command) == 0)                           \
    {                                                                            \
        spu->code[spu->ip] = constNameCommand;                                   \
        spu->ip++;                                                               \
        i += t_0 + SPACE;                                                        \
        char argument_recognizer[MAX_LABEL_SIZE] = "";                           \
        int t_1 = 0;                                                             \
        sscanf(buffer + i, "%s%n", argument_recognizer, &t_1);                   \
        if (isdigit(argument_recognizer[0]) != 0)                                \
        {                                                                        \
            spu->code[spu->ip] = atoi(argument_recognizer);                      \
            spu->ip++;                                                           \
            i += t_1 + SPACE;                                                    \
        }                                                                        \
        else                                                                     \
        {                                                                        \
            bool metka = searchLabel(spu, buffer, argument_recognizer, &i, t_1); \
            if (!metka)                                                          \
                sintaxError(3);                                                  \
        }                                                                        \
    }   

size_t asembler(SPU* spu)
{
    assert(spu);

    const char name_file[19] = "assembler_file.txt";
    FILE* assembler = fopen(name_file, "r");                         // ОТКРЫТИЕ ФАЙЛА ДЛЯ ЧТЕНИЯ 
    assert(assembler);

    size_t* size_file = getFileSize(name_file);                      // ПОЛУЧЕНИЕ РАЗМЕРА ФАЙЛА 

    spu->code = (double*)calloc(*size_file * 3, sizeof(double));     // СОЗДАНИЕ МАССИВА code
    assert(spu->code);

    char* buffer = readFileToBuffer(assembler, *size_file);          // СОЗДАНИЕ БУФФЕРА
    assert(buffer);

    FCLOSE(assembler);

    char command_recognizer[10] = "";
    int i = 0;  

    findLabel(spu, buffer, &i, command_recognizer);
    //pDump(*spu);
    //abort();
    i = 0;
    spu->ip = 0;

    while (buffer[i] != '\0')
    {   
        int t_0 = 0;
        sscanf(buffer + i, "%s%n", command_recognizer, &t_0);

        if (strcmp(command_recognizer, "push") == 0)
        {
            spu->code[spu->ip] = COMMAND_PUSH; 
            spu->ip++;
            i += t_0 + SPACE;

            char argument_recognizer[3] = ""; 
            int t_1 = 0;

            sscanf(buffer + i, "%s%n", argument_recognizer, &t_1);
            ARG_RECOGNIZER arg = argInit(argument_recognizer);

            if (argument_recognizer[0] == '[')
            {
                workWithRAM(spu, argument_recognizer, &i, t_1);
                continue;
            }
            else if (checkArg(arg))
            {
                workWithNum(spu, argument_recognizer, &i, &t_1);
                continue;
            }
            else 
            {
                workWithReg(spu, &i, arg);
                continue;
            }
        }    
            
        else if (strcmp(command_recognizer, "pop") == 0)
        {
            spu->code[spu->ip] = COMMAND_POP; 
            spu->ip++;
            i += t_0 + SPACE;

            char argument_recognizer[3] = ""; 
            int t_1 = 0;

            sscanf(buffer + i, "%s%n", argument_recognizer, &t_1);
            ARG_RECOGNIZER arg = argInit(argument_recognizer);

            if (argument_recognizer[0] == '[')
            {
                workWithRAM(spu, argument_recognizer, &i, t_1);
                continue;
            }
            else 
            {
                workWithReg(spu, &i, arg);
                continue;
            }   
        }

        else if (strncmp(command_recognizer, "hlt", 3) == 0)                         \
        {                                                                            \
            spu->code[spu->ip] = COMMAND_HLT;                                        \
            spu->ip++;                                                               \
            i += t_0 + SPACE;                                                        \
            continue;                                                                \
        }

        ONE_ARGUMENT_COMMAND("dump" , COMMAND_DUMP )
        ONE_ARGUMENT_COMMAND("sdump", COMMAND_SDUMP)
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
        ONE_ARGUMENT_COMMAND("ctg"  , COMMAND_CTG  )
        ONE_ARGUMENT_COMMAND("ret"  , COMMAND_RET  )
        
        JUMP_TYPE_COMMAND("call", COMMAND_CALL)
        JUMP_TYPE_COMMAND("jmp" , COMMAND_JMP )  
        JUMP_TYPE_COMMAND("ja"  , COMMAND_JA  )
        JUMP_TYPE_COMMAND("jb"  , COMMAND_JB  )
        JUMP_TYPE_COMMAND("jae" , COMMAND_JAE )
        JUMP_TYPE_COMMAND("jbe" , COMMAND_JBE )
        JUMP_TYPE_COMMAND("je"  , COMMAND_JE  )
        JUMP_TYPE_COMMAND("jhe" , COMMAND_JHE )

        else 
        {
            size_t len = strlen(command_recognizer);
            i += len;
            continue;         
        }
    } 

    FREE(buffer);
    return *size_file;
}

size_t* getFileSize(const char* name_file)
{
    assert(name_file);

    struct stat st;
    stat(name_file, &st); 
    size_t* size_file = (size_t*)calloc(1, sizeof(size_t*));
    assert(size_file);

    *size_file = st.st_size;                                                                          
    return size_file; 
}

char* readFileToBuffer(FILE* file, size_t size_file)
{
    assert(file);
    assert(size_file);

    char* buffer = (char*)calloc(size_file, sizeof(char));    // СОЗДАНИЕ БУФФЕРА
    assert(buffer);

    size_file = fread(buffer, sizeof(char), size_file, file); // ЗАПОЛЕНИЕ БУФФЕРА

    return buffer;
}

int searchRegister(ARG_RECOGNIZER arg)
{
    if (arg.a == 0) return REGISTER_ax;
    if (arg.b == 0) return REGISTER_bx;
    if (arg.c == 0) return REGISTER_cx;
    if (arg.d == 0) return REGISTER_dx;
    if (arg.e == 0) return REGISTER_ex;
    if (arg.f == 0) return REGISTER_fx;
    if (arg.g == 0) return REGISTER_gx;
    if (arg.h == 0) return REGISTER_hx;
    return REGISTER_NULL;
}

void createLabel(SPU* spu, int* i, char command_recognizer[])
{
    assert(spu);
    assert(i);
    assert(command_recognizer);

    defineLabel(spu, command_recognizer);

    size_t len = strlen(command_recognizer);
    *i += len;

    addLabel(spu, command_recognizer, len);
}

void defineLabel(SPU* spu, char command_recognizer[])
{
    assert(spu);
    assert(command_recognizer);

    char temp = 'a';
    int j = 0;
    while (j < MAX_LABEL_SIZE)
    {
        if (command_recognizer[j] == ':')
        { 
            temp = command_recognizer[j];
            j = MAX_LABEL_SIZE;
            continue;
        }
        j++;
        if (j == MAX_LABEL_SIZE)
            sintaxError(1);
    }
    if (temp != ':')
        sintaxError(2);
}

void findLabel(SPU* spu, char* buffer, int* i, char command_recognizer[])
{
    assert(spu);
    assert(buffer);
    assert(command_recognizer);

    while (buffer[*i] != '\0')
    {   
        sscanf(buffer + (*i), "%s", command_recognizer);
        size_t command_recognizer_len = strlen(command_recognizer);

        spu->ip += ipMove(command_recognizer);
        //printf(MANG "command_recognizer = %s\n" RESET, command_recognizer);
        //printf(GREEN "command_recognizer[command_recognizer_len - 1] = %c\n" RESET, command_recognizer[command_recognizer_len - 1]);
        if (command_recognizer[command_recognizer_len - 1] == ':')
        {
            //printf(YELLOW "ADD LABEL %s\n" RESET, command_recognizer);
            addLabel(spu, command_recognizer, command_recognizer_len);
            (*i) += command_recognizer_len + SPACE;
        }
        else    
            (*i) += command_recognizer_len + SPACE;
    }
}

void addLabel(SPU* spu, char command_recognizer[], size_t len)
{
    assert(spu);
    assert(command_recognizer);

    command_recognizer[len - 1] = '\0';  // СТИРАНИЕ ДВОЕТОЧИЯ ИЗ НАЗВАНИЯ МЕТКИ
    spu->lb[spu->labelCounter].nameLabel = strdup(command_recognizer);
    spu->lb[spu->labelCounter].ipLabel = spu->ip + 1;
    spu->labelCounter++;
}

ARG_RECOGNIZER argInit(char* argument_recognizer)
{
    assert(argument_recognizer);

    ARG_RECOGNIZER arg = {};
    arg.a = strncmp(argument_recognizer, "ax", 2);
    arg.b = strncmp(argument_recognizer, "bx", 2);
    arg.c = strncmp(argument_recognizer, "cx", 2);
    arg.d = strncmp(argument_recognizer, "dx", 2);
    arg.e = strncmp(argument_recognizer, "ex", 2);
    arg.f = strncmp(argument_recognizer, "fx", 2);
    arg.g = strncmp(argument_recognizer, "gx", 2);
    arg.h = strncmp(argument_recognizer, "hx", 2);
    return arg;
}

bool checkArg(ARG_RECOGNIZER arg)
{
    if (arg.a != 0 && arg.b != 0 && arg.c != 0 && arg.d != 0 && arg.e != 0 && arg.f != 0 && arg.g != 0 && arg.h != 0)
        return true;
    else 
        return false;    
}

void sintaxError(int num)
{
    printf(RED "SINTAX ERROR %d\n" RESET, num);
    abort();
}

bool searchLabel(SPU* spu, char* buffer, char argument_recognizer[], int* i, int t_1)
{
    assert(spu);
    assert(buffer);
    assert(argument_recognizer);

    bool metka = false;
    for (int y = 0; y < spu->labelCounter; y++)
    {
        if (strcmp(argument_recognizer, spu->lb[y].nameLabel) == 0)
        {
            spu->code[spu->ip] = spu->lb[y].ipLabel;
            spu->ip++;
            metka = true;
            (*i) += t_1 + SPACE;
            continue;
        }
    }
    return metka;
} 

int ipMove(char command_recognizer[])
{
    assert(command_recognizer);
    int ip = 0;
    if (strcmp(command_recognizer, "push" ) == 0) ip += 3;
    if (strcmp(command_recognizer, "pop"  ) == 0) ip += 3;
    if (strcmp(command_recognizer, "hlt"  ) == 0) ip += 1;
    if (strcmp(command_recognizer, "dump" ) == 0) ip += 1;
    if (strcmp(command_recognizer, "sdump") == 0) ip += 1;
    if (strcmp(command_recognizer, "add"  ) == 0) ip += 1;
    if (strcmp(command_recognizer, "sub"  ) == 0) ip += 1;
    if (strcmp(command_recognizer, "mul"  ) == 0) ip += 1;
    if (strcmp(command_recognizer, "div"  ) == 0) ip += 1;
    if (strcmp(command_recognizer, "sqrt" ) == 0) ip += 1;
    if (strcmp(command_recognizer, "out"  ) == 0) ip += 1;
    if (strcmp(command_recognizer, "in"   ) == 0) ip += 1;
    if (strcmp(command_recognizer, "sin"  ) == 0) ip += 1;
    if (strcmp(command_recognizer, "cos"  ) == 0) ip += 1;
    if (strcmp(command_recognizer, "ret"  ) == 0) ip += 1;
    if (strcmp(command_recognizer, "jmp"  ) == 0) ip += 2;
    if (strcmp(command_recognizer, "ja"   ) == 0) ip += 2;
    if (strcmp(command_recognizer, "jb"   ) == 0) ip += 2;
    if (strcmp(command_recognizer, "jae"  ) == 0) ip += 2;
    if (strcmp(command_recognizer, "jbe"  ) == 0) ip += 2;
    if (strcmp(command_recognizer, "je"   ) == 0) ip += 2;
    if (strcmp(command_recognizer, "jhe"  ) == 0) ip += 2;
    if (strcmp(command_recognizer, "call" ) == 0) ip += 2;

    return ip;
}

void workWithRAM(SPU* spu, char argument_recognizer[], int* i, int t_1)
{
    assert(spu);
    assert(argument_recognizer);
    (*i)++;
    spu->code[spu->ip] = ARGTYPE_M;
    spu->ip++;
    argument_recognizer[t_1 - 1] = '\0';
    if (!isalpha(argument_recognizer[1]))
    {
        double argument = atoi(argument_recognizer + 1);

        if (argument >= RAM_SIZE || argument < 0)
            sintaxError(6);
        
        spu->code[spu->ip] = argument;
        spu->ip++;

        (*i) += t_1;
    }
    else
    {
        spu->code[spu->ip - 1] += ARGTYPE_R;
        ARG_RECOGNIZER argRam = argInit(argument_recognizer + 1);
        int registr = searchRegister(argRam);

        if (!registr)
            sintaxError(8);

        spu->code[spu->ip] = registr;
        spu->ip++;
        (*i) += t_1;
    }
}

void workWithReg(SPU* spu, int* i, ARG_RECOGNIZER arg)
{
    assert(spu);
    (*i) += 3;
    int registr = searchRegister(arg);
    spu->code[spu->ip] = ARGTYPE_R;
    spu->ip++;
    spu->code[spu->ip] = registr;
    spu->ip++;
}

void workWithNum(SPU* spu, char argument_recognizer[], int* i, int* t_1)
{
    assert(spu);
    assert(argument_recognizer);

    int argument = 0;
    sscanf(argument_recognizer, "%d%n", &argument, t_1);
    spu->code[spu->ip] = ARGTYPE_I; 
    spu->ip++;
    (*i) += SPACE;
    spu->code[spu->ip] = argument; 
    spu->ip++;
    (*i) += (*t_1);
}