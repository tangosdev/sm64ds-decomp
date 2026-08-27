#include "types.h"
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 addr, u32 size);
extern void _ZN4CP1516DrainWriteBufferEv(void);
extern void func_020688fc(u32 dest, u32 src, u32 arg2);

/* func_02067f2c - flush destination buffer, drain write buffer, then call DMA transfer */
void func_02067f2c(u32 size, u32 arg1, u32 dest)
{
    u32 alignedSize = (size + 0x1f) & ~0x1fu;
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(dest, alignedSize);
    _ZN4CP1516DrainWriteBufferEv();
    func_020688fc(dest, size, arg1);
}