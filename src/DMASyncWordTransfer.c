/* DMASyncWordTransfer at 0x0205a160
 * Synchronous 32-bit DMA transfer: waits for the DMA channel to be free,
 * starts a word copy, then waits again until completion.
 */

#include "nitro/hw/registers.h"

extern void DMAStartTransferFB(u8 channel, u32 src, u32 dest, u32 ctrl);

void DMASyncWordTransfer(u8 channel, u32 src, u32 dest, u32 len)
{
    volatile u32 *cnt;
    if (len == 0)
        return;
    cnt = REG_DMA_CNT_PTR(channel);
    while (*cnt & DMA_CONTROL_ENABLE)
        ;
    DMAStartTransferFB(channel, src, dest,
                       (len >> 2) | DMA_CONTROL_ENABLE | DMA_CONTROL_32_BIT);
    while (*cnt & DMA_CONTROL_ENABLE)
        ;
}
