//cpp
#include "types.h"
#include "CP15.h"

extern "C" {

extern int MBi_SendMP(s32 dest, s32 src, s32 arg2);

/* NitroSDK MBi_BlockHeaderEnd (ntrtwl's libraries/mb/src/mb_common.c): flush
 * the send buffer to a 32-byte multiple, drain the write buffer, then send it
 * with MBi_SendMP. */
int MBi_BlockHeaderEnd(u32 body_len, u32 pollbmp, u32 sendbuf) {
    u32 alignedSize = (body_len + 0x1f) & ~0x1fu;
    CP15::FlushAndInvalidateDataCache(sendbuf, alignedSize);
    CP15::DrainWriteBuffer();
    return MBi_SendMP(sendbuf, body_len, pollbmp);
}
}
