#include "types.h"
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 addr, u32 size);

extern u32 data_0209e674;
extern u32 data_0209ea74;

/* OAM::Flush - flush main and sub OAM DMA buffers from data cache */
void _ZN3OAM5FlushEv(void)
{
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)&data_0209e674, 0x400);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)&data_0209ea74, 0x400);
}