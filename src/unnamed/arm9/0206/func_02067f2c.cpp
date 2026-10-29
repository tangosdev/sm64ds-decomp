//cpp
#include "types.h"
#include "CP15.h"

extern "C" {

extern void func_020688fc(u32 dest, u32 src, u32 arg2);

/* func_02067f2c - flush destination buffer, drain write buffer, then call DMA transfer */
void func_02067f2c(u32 size, u32 arg1, u32 dest)
{
    u32 alignedSize = (size + 0x1f) & ~0x1fu;
    CP15::FlushAndInvalidateDataCache(dest, alignedSize);
    CP15::DrainWriteBuffer();
    func_020688fc(dest, size, arg1);
}
}
