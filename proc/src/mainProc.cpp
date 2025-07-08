#include "../headers/proc.hpp"
#include <myLib.hpp>

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