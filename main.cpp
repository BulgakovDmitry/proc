#include "proc.h"
#include "procAsm.h"

int main(void)
{
    SPU* spu = spuInit();  
    size_t size_file = asembler(spu); 
    run(spu, size_file);
    spuDelete(spu);
    printDebug(999);
    return 0;
}
