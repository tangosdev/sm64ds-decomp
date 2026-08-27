//cpp
// @symbol _ZN3OAM5FlushEv
#include "OAM.h"

/* OAM::Flush() at 0x02021864 -- static, no `this`.
 *
 * Flushes the two 0x400-byte OAM DMA staging buffers -- main at 0x0209e674, sub at
 * 0x0209ea74 -- out of the data cache so the DMA that follows reads what the CPU
 * just wrote. 0x400 is 128 entries of 8 bytes, the full hardware OAM.
 *
 * CP15::FlushAndInvalidateDataCache stays an `extern "C"` free function: it is not
 * a member of OAM, and CP15 is one of the layout-free SDK namespaces
 * (plan-cpp-language-mode.md Phase 1) that has not been migrated yet.
 */
extern "C" {
void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 addr, u32 size);
extern u32 data_0209e674;  /* main OAM staging buffer */
extern u32 data_0209ea74;  /* sub OAM staging buffer */
}

void OAM::Flush()
{
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)&data_0209e674, 0x400);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)&data_0209ea74, 0x400);
}
