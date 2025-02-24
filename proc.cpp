#include <stdint.h>

#include "proc.h"
//#include "procAsm.h"

static int  getArgPush (SPU* spu);
static int  getArgPop  (SPU* spu);

static void pDump(SPU spu);
static void jump (SPU* spu);

static PairOfNum getPairOfNumbersFromStack(SPU* spu); 

#define STANDART_JMP(spu) \
    {                     \
        jump(spu);        \
        spu->ip--;        \
        break;            \
    }                     \

#define MODIFIED_JMP(spu, codeOp)       \
    {                                   \
        double a = StackPop(&spu->stk); \
        double b = StackPop(&spu->stk); \
        StackPush(&spu->stk, b);        \
        StackPush(&spu->stk, a);        \
        if (int(b) codeOp int(a))       \
            STANDART_JMP(spu)           \
        else                            \
            spu->ip += 2;               \
        break;                          \
    }             

#define JUMP_COMMON(spu, flagCond, codeOp) \
    {                                      \
        assert(spu);                       \
        if (!flagCond)                     \
            STANDART_JMP(spu)              \
        else                               \
            MODIFIED_JMP(spu, codeOp)      \
    }                                                                  

#define STANDART_BINARY_OPERATION(operation)             \
    {                                                    \
        PairOfNum pair = getPairOfNumbersFromStack(spu); \
        StackPush(&spu->stk, pair.b operation pair.a);   \
        spu->ip++;                                       \
        break;                                           \
    }

#define STANDART_UNARY_OPERATION(operation) \
    {                                       \
        double a = StackPop(&spu->stk);     \
        StackPush(&spu->stk, operation(a)); \
        spu->ip++;                          \
        break;                              \
    } 

int main(void)
{
    FILE* asmCodeFile = fopen(ASM_CODE_FILE, "r");
    assert(asmCodeFile);

    SPU* spu = spuInit(asmCodeFile);
    assert(spu);

    run(spu, spu->codeFileSize);
    spuDelete(spu);
    DBG(SUCCESS);
    return 0;
}

int run(SPU* spu, size_t sizeFile)
{
    double argument = 0;
    spu->ip = 0;

    while(true)
    {
        if ((int)sizeFile <= spu->ip + 1) // проверка на выход за границы code
        {
            printf(RED "\nERROR: THE CODE OVERFILLED\n" RESET); 
            return EXIT_FAILURE;
        }
        /*uint64_t*/double command = (/*(uint64_t*)*/(spu->code))[spu->ip];
        switch ((int)command)
        {
            case COMMAND_PUSH: 
            {
                StackPush(&spu->stk, getArgPush(spu));
                break;
            }
            case COMMAND_POP:  
            {
                getArgPop(spu);
                StackPop(&spu->stk);
                break;
            }
            case COMMAND_JMP :  JUMP_COMMON(spu, 0, ==);
            case COMMAND_JA  :  JUMP_COMMON(spu, 1, > );
            case COMMAND_JB  :  JUMP_COMMON(spu, 1, < );
            case COMMAND_JAE :  JUMP_COMMON(spu, 1, >=);
            case COMMAND_JBE :  JUMP_COMMON(spu, 1, <=);
            case COMMAND_JE  :  JUMP_COMMON(spu, 1, ==);
            case COMMAND_JHE :  JUMP_COMMON(spu, 1, !=);

            case COMMAND_CALL:
            {
                StackPush(&spu->recStk, spu->ip + JUMP_STEP);
                jump(spu);                      
                spu->ip--;
                break;
            }
            case COMMAND_RET:
            {
                spu->ip = (int)StackPop(&spu->recStk);
                break;
            }
            case COMMAND_IN:
            {
                double a = 0;
                scanf("%lg", &a);
                StackPush(&spu->stk, a);
                spu->ip++;
                break;
            }

            case COMMAND_OUT:
            {
                printf("%sOUT%s:%s %lg%s\n", GREEN, RED, MANG, StackPop(&spu->stk), RESET);
                spu->ip++;
                break;
            }
            case COMMAND_ADD: STANDART_BINARY_OPERATION(+);
            case COMMAND_SUB: STANDART_BINARY_OPERATION(-);
            case COMMAND_MUL: STANDART_BINARY_OPERATION(*);
            case COMMAND_DIV: STANDART_BINARY_OPERATION(/);
           
            case COMMAND_SQRT: STANDART_UNARY_OPERATION(sqrt);
            case COMMAND_SIN : STANDART_UNARY_OPERATION(sin);
            case COMMAND_COS : STANDART_UNARY_OPERATION(cos);
            case COMMAND_TG  : STANDART_UNARY_OPERATION(tan);
            case COMMAND_CTG : STANDART_UNARY_OPERATION(1 / tan);
            
            case COMMAND_DUMP:
            {
                spu->ip++;
                pDump(*spu);
                break;
            }
            case COMMAND_SDUMP:
            {
                spu->ip++;
                StackDump(&spu->stk, INFO());
                break;
            }
            case COMMAND_HLT:
            {
                printf("%sHLT%s\n", RED, RESET);
                spu->ip++;
                return EXIT_SUCCESS;
                break;
            } 
            case COMMAND_NULL:
            {
                printf("%sNULL_COMMAND%s\n", RED, RESET);
                pDump(*spu);
                printf(MANG "code[ip-2] = %lg, code[ip-1] = %lg, code[ip] = %lg, code[ip+1] = %lg\n" RESET, spu->code[spu->ip - 2], spu->code[spu->ip - 1], spu->code[spu->ip], spu->code[spu->ip + 1]);
                return EXIT_FAILURE;
                break;
            } 
            default:
            {
                printf("%sERROR: undefined command%s\n", RED, RESET);
                printf("%scommand = %lg/*llX*/ argument = %lg%s\n",BLUE, command, argument, RESET);
                return EXIT_FAILURE;
                break; 
            }        
        }  
    }
}

#undef MODIFIED_JMP
#undef STANDART_JMP
#undef JUMP_COMMON

#undef STANDART_BINARY_OPERATION
#undef STANDART_UNARY_OPERATION

static void pDump(SPU spu)
{
    printf("%s___ProcDump___~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s\n", RED, RESET);
    printf("%scode     %s: %s", GREEN, RED, RESET);
    for (int q = 0; q < spu.ip; q++) 
        printf("%s%lg   %s", CEAN, spu.code[q], RESET);
    putchar('\n');

    printf("%sip%s       :%s %d %s\n", GREEN, RED, CEAN, spu.ip, RESET);

    printf("%sSTACK%s    :%s ", GREEN, RED, RESET);
    for (size_t q = 0; q < spu.stk.size; q++) 
        printf("%s%lg  %s ", CEAN, spu.stk.data[q + 1], RESET);
    putchar('\n');

    printf("%sSTACK_REC%s:%s ", GREEN, RED, RESET);
    for (size_t q = 0; q < spu.recStk.size; q++) 
        printf("%s%lg  %s ", CEAN, spu.recStk.data[q + 1], RESET);
    putchar('\n');
    
    printf("%sRAM%s      :%s ", GREEN, RED, RESET);
    for (size_t q = 0; q < RAM_SIZE; q++) 
        printf("%s%lg  %s ", CEAN, spu.ram[q], RESET);
    putchar('\n');
    

    printf("%sREGISTERS%s: %sax%s =%s %lg%s | %sbx%s = %s%lg%s | %scx %s= %s%lg%s | %sdx%s =%s %lg%s | %sex%s =%s %lg%s | " 
                          "%sfx%s =%s %lg%s | %sgx%s = %s%lg%s | %shx %s= %s%lg%s | %s \n", GREEN, RED, 
                        BLUE, MANG, CEAN, spu.registers[REGISTER_ax], RED, BLUE, MANG, CEAN, spu.registers[REGISTER_bx], RED, 
                        BLUE, MANG, CEAN, spu.registers[REGISTER_cx], RED, BLUE, MANG, CEAN, spu.registers[REGISTER_dx], RED, 
                        BLUE, MANG, CEAN, spu.registers[REGISTER_ex], RED, BLUE, MANG, CEAN, spu.registers[REGISTER_fx], RED, 
                        BLUE, MANG, CEAN, spu.registers[REGISTER_gx], RED, BLUE, MANG, CEAN, spu.registers[REGISTER_hx], RED, RESET);
/*
    printf("%sLABELS   %s:%s", GREEN, RED, RESET);
    for (int w = 0; w < spu.labelCounter && spu.lb[w].nameLabel[0] != '\0'; w++)
        printf(" %s%s %s= %s%zu%s |%s", BLUE, spu.lb[w].nameLabel, MANG, CEAN, spu.lb[w].ipLabel, RED, RESET);
    putchar('\n');
*/
    printf("%s~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s\n", RED, RESET);
}

SPU* spuInit(FILE* asmCodeFile)
{
    SPU* spu = (SPU*)calloc(1, sizeof(SPU));
    assert(spu);

    STACK_CTOR(&spu->stk);
    STACK_CTOR(&spu->recStk);

    spu->registers = (double*)calloc(NUMBER_OF_REGISTERS + 1, sizeof(double)); // СОЗДАНИЕ ЯЧЕЕЙ ДЛЯ РЕГИСТРОВ
    assert(spu->registers);

    for (int s = 0; s < NUMBER_OF_REGISTERS; s++)  // ЗАПОЛНЕНИЕ ПЕРВОНАЧАЛЬНЫМИ ЗНАЧЕНИЯМИ (НУЛЯМИ)
        spu->registers[s + 1] = 0;

    spu->ram = (double*)calloc(RAM_SIZE, sizeof(double));
    assert(spu->ram);

    spu->codeFileSize = getFileSize(asmCodeFile);

    spu->code = (double*)calloc(spu->codeFileSize, sizeof(double));
    assert(spu->code);

    //for (int i = 0; fscanf(asmCodeFile, "%lg", spu->code + i) != EOF; i++)
    //    ;
    fread(spu->code, sizeof(double), spu->codeFileSize, asmCodeFile);
    spu->ip = 0;

    return spu; 
}

void spuDelete(SPU* spu)
{
    assert(spu);

    if (spu->code)
        FREE(spu->code);

    if (spu->registers)
        FREE(spu->registers);

    if (spu->ram)
        FREE(spu->ram);
    
    spu->ip           = 0;

    StackDtor(&spu->stk);
    StackDtor(&spu->recStk);
    FREE(spu);
}

static PairOfNum getPairOfNumbersFromStack(SPU* spu)
{
    assert(spu);

    PairOfNum pair = {};

    pair.a = StackPop(&spu->stk);
    pair.b = StackPop(&spu->stk);

    return pair;
}

static int getArgPush(SPU* spu)
{
    assert(spu);

    spu->ip++;
    int argument = (int)spu->code[spu->ip]; 
    spu->ip++; 
    int result  = BAD_ARG; 
    if ((argument & ARGTYPE_M) && (argument & ARGTYPE_R))
    {
        result  = (int)spu->ram[(int)spu->registers[(int)spu->code[spu->ip]]];  
        spu->ip++;  
    }
    else if (argument & ARGTYPE_I) 
    {
        result  = (int)spu->code[spu->ip];        
        spu->ip++; 
    }
    else if (argument & ARGTYPE_R) 
    {
        result += (int)spu->registers[(int)spu->code[spu->ip]];   
        spu->ip++;
    }
    else if (argument & ARGTYPE_M)
    {
        result = (int)spu->ram[(int)spu->code[spu->ip]];
        spu->ip++;
    }

    return result;
}


//TODO val as arg

static int getArgPop(SPU* spu)
{
    assert(spu);

    spu->ip++;
    int argument = (int)spu->code[spu->ip]; 
    spu->ip++;

    if ((argument & ARGTYPE_M) && (argument & ARGTYPE_R))
        spu->ram[(int)spu->registers[(int)spu->code[spu->ip]]] = StackGet(&spu->stk);
    else if (argument & ARGTYPE_R) 
        spu->registers[(int)spu->code[spu->ip]] = StackGet(&spu->stk);
    else if (argument & ARGTYPE_M)
        spu->ram[(int)spu->code[spu->ip]] = StackGet(&spu->stk);
    
    spu->ip++;

    return EXIT_SUCCESS;
}

static void jump (SPU* spu)
{
    assert(spu);
    spu->ip = (int)spu->code[spu->ip+1];
}