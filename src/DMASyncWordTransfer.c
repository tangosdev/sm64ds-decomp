#include "types.h"
/* DMASyncWordTransfer at 0x0205a160
 * Synchronous 32-bit DMA transfer: waits for the DMA channel to be free,
 * starts a word copy, then waits again until completion.
 */
extern void DMAStartTransferFB(u8 channel, u32 src, u32 dest, u32 ctrl);

static volatile u32* const DMA_BASE = (volatile u32*)0x040000b0;

void DMASyncWordTransfer(u8 channel, u32 src, u32 dest, u32 len)
{
    volatile u32 *cnt;
    if (len == 0)
        return;
    cnt = DMA_BASE + (channel * 3 + 2);
    while (*cnt & 0x80000000)
        ;
    DMAStartTransferFB(channel, src, dest, (len >> 2) | 0x84000000);
    while (*cnt & 0x80000000)
        ;
}
