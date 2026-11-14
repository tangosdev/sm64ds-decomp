/*
 * NitroSDK libraries/mb/src/mb_common.c (ntrtwl's NitroSDK): the multiboot
 * code shared by parent and child.
 *
 * The cartridge links two of its functions, MBi_calc_cksum (0x02067ee4) and
 * MBi_BlockHeaderEnd (0x02067f2c); mwcc emits C bodies in reverse source
 * order, so the higher-addressed MBi_BlockHeaderEnd comes first here.
 * MBi_BlockHeaderBegin and MB_GetParentSystemBufSize /
 * MB_GetChildSystemBufSize are not in the ROM; the debug-output macros are
 * empty in a release build.
 */
#include "types.h"
#include "nitro/os/cache.h"
#include "private/mb_common.h"
#include "private/mb_wm_base.h"

#define MY_ROUND(n, a) (((u32)(n) + (a) - 1) & ~((a) - 1))

int MBi_BlockHeaderEnd(int body_len, u16 pollbmp, u32 *sendbuf) {
    DC_FlushRange(sendbuf, MY_ROUND(body_len, 32));
    DC_WaitWriteBufferEmpty();

    return _Z10MBi_SendMPPKvii(sendbuf, body_len, pollbmp);
}

u16 MBi_calc_cksum(const u16 *buf, int length) {
    u32 sum;
    int nwords = length >> 1;

    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return (u16)(sum ^ 0xffff);
}
