//cpp
#include "types.h"
#include "nitro/os/cache.h"

extern "C" {

extern int func_020688fc(s32 dest, s32 src, s32 arg2);

/* func_02067f2c - flush destination buffer, drain write buffer, then call DMA transfer */
int func_02067f2c(u32 size, u32 arg1, u32 dest) {
    u32 alignedSize = (size + 0x1f) & ~0x1fu;
    DC_FlushRange((void *)dest, alignedSize);
    DC_WaitWriteBufferEmpty();
    return func_020688fc(dest, size, arg1);
}
}
