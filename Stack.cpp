#include "Stack.h"
////////////////////___СОЗДАНИЕ И ОЧИСТКА СТЕКА___/////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*---------------ФУНКЦИЯ КОНСТРУКТОР ДЛЯ СОЗДАНИЯ СТЕКА------------------------------------------------------------------------------------------*/
int StackCtor(Stack_t* stk)
{
    if (stk == NULL)
    {
        Info_locate StackCtor_info = INFO();
        printf("%s___ASSERT___~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s\n", RED, RESET);
        printf("%s!!!   %sFile    %s%s%s\n", RED, MANG, BLUE, StackCtor_info.file, RESET);
        printf("%s!!!   %sLine    %s%zu%s\n", RED, MANG, BLUE, StackCtor_info.line, RESET);
        printf("%s!!!   %sFunc    %s%s%s\n", RED, MANG, BLUE, StackCtor_info.func, RESET);
        printf("%s~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s\n", RED, RESET);
        abort();
    }
    stk->L_STACK_KANAR = L_STACK_KANAR;
    stk->koef_capacity = 2;
    stk->size = 0;
    stk->capacity = START_SIZE;

    stk->data = 0;
    stk->data = (StackElem_t*)calloc(stk->capacity + 2, sizeof(StackElem_t));
   
    stk->data[0] = L_DATA_KANAR;                  // УСТАНОВКА ЛЕВОЙ  КАНАРЕЙКИ НА data
    stk->data[stk->capacity + 1] = R_DATA_KANAR;  // УСТАНОВКА ПРАВОЙ КАНАРЕЙКИ НА data


    stk->error_status = 0;
    stk->R_STACK_KANAR = R_STACK_KANAR;
    StackCheckAssertError(stk);
    return 0;
}

/*--------------ФУНКЦИЯ ДЛЯ ЧИСТКИ ПАМЯТИ СТЕКА--------------------------------------------------------------------------------------------------*/
int StackDtor(Stack_t* stk)
{
    STACK_ASSERT(stk);
    FREE(stk->data);
    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////__ДОБАВЛЕНИЯ ИЛИ УДАЛЕНИЕ ЭЛЕМЕНТОВ, РАБОТА СО СТЕКОМ___///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*---------------ФУНКЦИЯ ДЛЯ ДОБАВЛЕНИЯ ЭЛЕМЕНТА СТЕКА-------------------------------------------------------------------------------------------*/
void StackPush(Stack_t* stk, StackElem_t elem)
{
    StackCheckAssertError(stk);
    if (stk->size >= stk->capacity)
    {    
        stk->data[0] = POISON;                  // УДАЛЕНИЕ СТАРОЙ ЛЕВОЙ КАНАРЕЙКИ
        stk->data[stk->capacity + 1] = POISON;  // УДАЛЕНИЕ СТРОЙ ПРАВОЙ КАНАРЕЙКИ (ИЗМЕНЕНИЕ НА POISON)

        stk->capacity = stk->capacity * stk->koef_capacity;
        stk->data = (StackElem_t*)realloc(stk->data, (stk->capacity + 2)* sizeof(StackElem_t) + 2);
        STACK_ASSERT(stk);
    }    
    stk->data[0] = L_DATA_KANAR;  // УСТАНОВКА НОВОЙ ЛЕВОЙ КАНАРЕЙКИ
    //stk->data[0] = R_DATA_KANAR;  // УСТАНОВКА НОВОЙ ЛЕВОЙ КАНАРЕЙКИ ***********
    stk->data[stk->capacity + 1] = R_DATA_KANAR;    // УСТАНОВКА НОВОЙ ПРАВОЙ КАНАРЕЙКИ
    stk->data[stk->size + 1] = elem;
    stk->size++;
    StackCheckAssertError(stk);
}

/*---------------ФУНКЦИЯ ДЛЯ УДАЛЕНИЯ ЭЛЕМЕНТА СТЕКА---------------------------------------------------------------------------------------------*/
StackElem_t StackPop(Stack_t* stk)
{
    StackCheckAssertError(stk);
    if ((stk->size < (0.5 * stk->capacity)) && stk->size > 1 /* && stk->capacity >= START_SIZE*/)
    {
        if (stk->capacity > START_SIZE)
        {
            stk->data[0]                 = POISON;  // УДАЛЕНИЕ СТАРОЙ ЛЕВОЙ КАНАРЕЙКИ
            stk->data[stk->capacity + 1] = POISON;  // УДАЛЕНИЕ СТРОЙ ПРАВОЙ КАНАРЕЙКИ (ИЗМЕНЕНИЕ НА POISON)
            stk->capacity = stk->capacity / stk->koef_capacity; // УМЕНЬШЕНИЕ capacity

            stk->data = (StackElem_t*)realloc(stk->data, (stk->capacity + 2) * sizeof(StackElem_t));
            STACK_ASSERT(stk);

            stk->data[0] = L_DATA_KANAR;                 // УСТАНОВКА НОВОЙ ЛЕВОЙ КАНАРЕЙКИ
            stk->data[stk->capacity + 1] = R_DATA_KANAR; // УСТАНОВКА НОВОЙ ПРАВОЙ КАНАРЕЙКИ
        }   
    }  
    if (stk->size == 0) {return StackOK;}

    StackElem_t temp = stk->data[stk->size /*- 1*/];   
    stk->data[stk->size] = 0;   
    stk->size--;
    StackCheckAssertError(stk);
    return temp;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////___ВСЕВОЗМОЖНЫЕ ПРОВЕРКИ СТЕКА НА ОШИБКИ, АТАКУ И КОРРЕКТНОСТЬ АДРЕСОВ/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*--------ФУНКЦИЯ ДЛЯ ПРОВЕРКИ НА ОШИБКИ И В СЛУЧАЕ ЧЕГО АССЕРТА СТЕКА---------------------------------------------------------------------------*/ 
void StackCheckAssertError(Stack_t* stk)
{
    STACK_ASSERT(stk);
    StackError(stk);
}

/*--------ФУНКЦИЯ ДЛЯ СОЗДАНИЯ МАКРОСА STACK_ASSERT ДЛЯ ПРОВЕРКИ ВАЛИДНОСТИ СТРУКТУРЫ И СТЕКА----------------------------------------------------*/
void StackAssertFunk(Stack_t* stk, const char* FILE, const char* FUNC, size_t LINE)
{   
    if (StackOk(stk) < 0)
    {
        printf("%s___ASSERT___~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s\n", RED, RESET);
        printf("%s!!!   %sFile    %s%s%s\n", RED, MANG, BLUE, FILE, RESET);
        printf("%s!!!   %sLine    %s%zu%s\n", RED, MANG, BLUE, LINE, RESET);
        printf("%s!!!   %sfunc    %s%s%s\n", RED, MANG, BLUE, FUNC, RESET);
        printf("%s~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s\n", RED, RESET);
        abort();
    }
}

/*-------------ПОДФУНКЦИЯ ФУНКЦИИ ASSERT---------------------------------------------------------------------------------------------------------*/
int StackOk(Stack_t* stk)
{
    if (stk == NULL)       {return InvalidStructurePointer;}
    if (stk->data == NULL) {return InvalidStackPointer;    }
    
    return StackOK;  
}

/*----------ФУНКЦИЯ ДЛЯ ОБРАБОТКИ ОШИБОК---------------------------------------------------------------------------------------------------------*/
void StackError(Stack_t* stk)
{
    STACK_ASSERT(stk);
    StackCheckError(stk);
    StackAssertError(stk);
    STACK_ASSERT(stk);
}

/*-----------ФУНКЦИЯ ДЛЯ ПРОВЕРКИ ОШИБОК---------------------------------------------------------------------------------------------------------*/
int StackCheckError(Stack_t* stk)
{
    STACK_ASSERT(stk);
    //if (stk->size < 0)                                {stk->error_status += IncorrectStackSize;    }
    if (stk->size > stk->capacity)                    {stk->error_status += SizeExceededCapacity;  }
    if (stk->L_STACK_KANAR != L_STACK_KANAR)          {stk->error_status += LeftAttackOnStructure; }
    if (stk->R_STACK_KANAR != R_STACK_KANAR)          {stk->error_status += RightAttackOnStructure;}
    if (stk->data[0] != L_DATA_KANAR)                 {stk->error_status += LeftAttackOnStack;     }
    if (stk->data[stk->capacity + 1] != R_DATA_KANAR) {stk->error_status += RightAttackOnStack;    } 
    STACK_ASSERT(stk);
    return StackOK;
}

/*-----------ФУНКЦИЯ ДЛЯ assert С ИНФОРМАЦИЕЙ ОБ ОШИБКАХ-----------------------------------------------------------------------------------------*/
void StackAssertError(Stack_t* stk)
{
    STACK_ASSERT(stk);
    if (stk->error_status & StackOK               ) {StackPrintError(RightAttackOnStack);           }

    if (stk->error_status & RightAttackOnStack    ) {StackPrintError(RightAttackOnStack);           }
   
    if (stk->error_status & LeftAttackOnStack     ) {StackPrintError(LeftAttackOnStack);            }
    
    if (stk->error_status & RightAttackOnStructure) {StackPrintError(RightAttackOnStructure);       }

    if (stk->error_status & LeftAttackOnStructure ) {StackPrintError(LeftAttackOnStructure);        }

    if (stk->error_status & SizeExceededCapacity  ) {StackPrintError(SizeExceededCapacity);         }

    if (stk->error_status & IncorrectStackSize    ) {StackPrintError(IncorrectStackSize);           }

    if (stk->error_status != StackOK              ) {StackPrintError(UndefinedStackError);  abort();}   
}

/*-----------ФУНКЦИЯ ДЛЯ ПЕЧАТИ ОШИБОК---------------------------------------------------------------------------------------------------------*/
int StackPrintError(int error)
{   
    //fprintf(StackError, "\t______ERRORS______\n");
    //size_t n_errors = 0; 
    switch (error)
    {
    case StackOK:
        return StackOK;
        break;    
    case LeftAttackOnStack:
        printf("%sERROR!   The left data_canary died%s\n", RED, RESET);
        break;
    case RightAttackOnStack:
        printf("%sERROR!   The left data_canary died%s\n", RED, RESET);
        break;
    case RightAttackOnStructure:
        printf("%sERROR!   The structure attaked right%s\n", RED, RESET);
        break;
    case LeftAttackOnStructure:
        printf("%sERROR!   The structure attaked left%s\n", RED, RESET);
        break;
    case SizeExceededCapacity:
        printf("%sERROR!   The size exceeded capacity%s\n", RED, RESET); 
        break;
    case IncorrectStackSize:
        printf("%sERROR!   The size of stack is incorrect%s\n", RED, RESET);
        break;
    default:
        printf("%sERROR!  The error not located%s\n", RED, RESET);
        return UndefinedStackError;
        break;
    }
    return StackOK;
}

/*-----------ФУНКЦИЯ ДЛЯ СОЗДАНИЯ МАКРОСА INFO, КОТОРЫЙ ПОЛУЧАЕТ ИНФОРМАЦИЮ О МЕСТЕ ВЫЗОВА-------------------------------------------------------*/
Info_locate Info(const char* FILE, size_t LINE, const char* FUNC)
{
    Info_locate info_l = {};
    info_l.file = FILE;
    info_l.line = LINE;
    info_l.func = FUNC;
    return info_l;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////___РАСПЕЧАТКА СОСТОЯНИЯ СТЕКА___///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------ФУНКЦИЯ ДЛЯ ДЕБАГА (РАСПЕЧАТКИ)----------------------------------------------------------------------------------------------------*/
void StackDump(Stack_t* stk, Info_locate info_l)
{
    STACK_ASSERT(stk);
    printf("%s___StackDump___~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s\n", RED, RESET);
    printf("%sStack_t %s[%s%p%s]%s\n", CEAN, GREEN, MANG, stk, GREEN, RESET);
    printf("%sCalled from %s%s %s: %s%zu %s(%s%s%s)  %s\n", CEAN, MANG, info_l.file, GREEN, RED, info_l.line, GREEN, BLUE, info_l.func, GREEN, RESET);
    printf("%sname 'stk' born at %s%s %s:%s %zu %s(%s%s%s)%s\n", CEAN, MANG, stk->info_Stack_t.file, GREEN, RED, stk->info_Stack_t.line, GREEN, BLUE, stk->info_Stack_t.func, GREEN, RESET);
    printf("%s{%sL_STACK_KANAR %s= %s%X%s", GREEN, BLUE, GREEN, RED, (int)stk->L_STACK_KANAR, RESET);
    printf("%s, %sR_STACK_KANAR %s= %s%X%s}%s\n", GREEN, BLUE, GREEN, RED, (int)stk->R_STACK_KANAR, GREEN, RESET);
    printf("%s{%sL_DATA_KANAR %s = %s%X%s", GREEN, BLUE, GREEN, RED, (int)stk->data[0], RESET);
    printf("%s, %sR_DATA_KANAR %s = %s%X%s}%s\n", GREEN, BLUE, GREEN, RED, (int)stk->data[stk->capacity + 1], GREEN, RESET);
    printf("%scapasity %s= %s%zu%s\n", BLUE, GREEN, RED, stk->capacity, RESET);
    printf("%ssize %s= %s%zu%s\n", BLUE, GREEN, RED, stk->size, RESET);
    printf("%sdata %s[%s%p%s]%s\n", CEAN, GREEN, MANG, stk->data, GREEN, RESET);
    StackPrint(stk);
    StackError(stk);
    printf("%s~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s\n", RED, RESET);
    STACK_ASSERT(stk);
}

/*-----------ФУНКЦИЯ ДЛЯ ПЕЧАТИ СТЕКА (ПОДФУНКЦИЯ ФУНКЦИИ StackDump)-----------------------------------------------------------------------------*/
void StackPrint(Stack_t* stk)
{
    StackCheckAssertError(stk);
    printf("%s{%s\n", GREEN, RESET);
    printf("  %sk%s[%s%3d%s]   = %s  %X %s\n", YELLOW, GREEN, MANG, 0, GREEN, RED, (int)stk->data[0], RESET);
    for (size_t i = 1; i < stk->size + 1; i++) 
    {
        printf("  %s*%s[%s%3zu%s]   = %s%3lg%s\n", YELLOW, GREEN, MANG, i, GREEN, RED, stk->data[i], RESET);
    }
    
    for (size_t j = 1; j < stk->capacity - stk->size + 1; j++) 
    {
        printf("   %s[%s%3zu%s]   = %s%5lg %s (%sPOISON%s)%s\n", GREEN, MANG, j + stk->size, GREEN, RED, POISON, GREEN, YELLOW, GREEN, RESET);
    }
    printf("  %sk%s[%s%3zu%s]   = %s  %X %s \n", YELLOW, GREEN, MANG, stk->capacity + 1, GREEN, RED, (int)stk->data[stk->capacity+1], RESET);
    printf("%s}%s\n", GREEN, RESET);
    StackCheckAssertError(stk);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
