/* func_01ffde98 at 0x01ffde98 (itcm)
 * Synchronous 32-bit DMA of a command list into the geometry command FIFO
 * (GXFIFO, 0x04000400 per GBATEK), split into chunks of at most 0x1d8 bytes.
 */

#include "nitro/hw/registers.h"

#define DMA_CONTROL_DEST_FIXED (2u << 21)

extern void DMAStartTransfer(u32 channel, void *src, void *dst, u32 cnt);

void func_01ffde98(u32 channel, void *src, u32 size)
{
    volatile u32 *dmaCtrl;
    void *fifo;
    u32 chunk;

    if (size == 0)
        return;

    dmaCtrl = REG_DMA_CNT_PTR(channel);

    while (*dmaCtrl & DMA_CONTROL_ENABLE)
        ;

    fifo = (void *)0x04000400;
    while (size != 0) {
        chunk = size > 0x1d8 ? 0x1d8 : size;
        DMAStartTransfer(channel, src, fifo,
                         (DMA_CONTROL_ENABLE | DMA_CONTROL_32_BIT |
                          DMA_CONTROL_DEST_FIXED) | (chunk >> 2));
        size -= chunk;
        src = (u8 *)src + chunk;
    }

    while (*dmaCtrl & DMA_CONTROL_ENABLE)
        ;
}
