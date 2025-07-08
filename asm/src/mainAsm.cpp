#include "../headers/asm.hpp"

int main()
{
    FILE* assembler = fopen(ASSEMBLER_FILE_NAME, "r");               // ОТКРЫТИЕ ФАЙЛА ДЛЯ ЧТЕНИЯ 
    assert(assembler);

    Asm* asem = asmCtor(assembler);
    assert(asem);
    
    asembler(asem, assembler);

    FILE* asmCodeFile = fopen(ASM_CODE_FILE, "w");
    assert(asmCodeFile);

    fwrite(asem->code, sizeof(double), asem->ip, asmCodeFile);

    asmDtor(asem);

    FCLOSE(asmCodeFile);
    FCLOSE(assembler);

    DBG(SUCCESS);
    return 0;
}