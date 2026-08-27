/* DMASyncHalfTransfer at 0x0205a10c
 * Synchronous DMA transfer of numHalfs halfwords.
 * Waits for previous DMA, starts transfer, waits for completion.
 */

#include "nitro/hw/registers.h"

extern void DMAStartTransferFB(u32 channel, void *src, void *dst, u32 cnt);

void DMASyncHalfTransfer(u32 channel, void *src, void *dst, u32 numHalfs)
{
    volatile u32 *dmaCtrl;
    u32 cnt;

    if (numHalfs == 0)
        return;

    dmaCtrl = REG_DMA_CNT_PTR(channel);

    while (*dmaCtrl & DMA_CONTROL_ENABLE)
        ;

    cnt = (numHalfs >> 1) | DMA_CONTROL_ENABLE;
    DMAStartTransferFB(channel, src, dst, cnt);

    while (*dmaCtrl & DMA_CONTROL_ENABLE)
        ;
}
