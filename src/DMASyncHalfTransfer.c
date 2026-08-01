#include "types.h"
/* DMASyncHalfTransfer at 0x0205a10c
 * Synchronous DMA transfer of numHalfs halfwords.
 * Waits for previous DMA, starts transfer, waits for completion.
 */
extern void DMAStartTransferFB(u32 channel, void *src, void *dst, u32 cnt);

void DMASyncHalfTransfer(u32 channel, void *src, void *dst, u32 numHalfs)
{
    volatile u32 *dmaCtrl;
    u32 cnt;
    u32 *dmaBase;

    if (numHalfs == 0)
        return;

    dmaBase = (u32 *)0x040000b0;
    dmaCtrl = dmaBase + (channel * 3 + 2);

    while (*dmaCtrl & 0x80000000)
        ;

    cnt = (numHalfs >> 1) | 0x80000000;
    DMAStartTransferFB(channel, src, dst, cnt);

    while (*dmaCtrl & 0x80000000)
        ;
}
