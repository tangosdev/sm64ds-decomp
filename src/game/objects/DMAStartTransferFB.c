/* DMAStartTransferFB at 0x01ffde00 (ITCM)
 *
 * Arms one DMA channel -- source, destination, control -- with interrupts
 * masked, then issues two dummy reads of DMA0SAD.
 *
 * It is a twin of DMAStartTransfer at 0x01ffde50: the two are
 * instruction-for-instruction identical except for those two reads. The reads
 * cover the DS's two-cycle DMA start delay, which is why exactly the three
 * synchronous helpers call this variant (DMASyncHalfTransfer 0x0205a144,
 * DMASyncWordTransfer 0x0205a198, DMASyncFillTransfer 0x0205a1fc): each polls
 * DMAxCNT's enable bit on the very next instruction, so the channel has to
 * have taken the bus before the poll. The callback/asynchronous paths
 * (func_02059fd0, func_0205a064, func_0205a290) call the plain twin instead.
 *
 * The barrier reads DMA0SAD, not the armed channel's own register -- any read
 * of the DMA register block costs the two cycles.
 */

#include "types.h"
#include "nitro/hw/registers.h"

extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 savedState);

/* The three per-channel DMA registers: 12 bytes, striding from 0x040000b0. */
typedef struct DMAChannelRegs {
    u32 src;
    u32 dst;
    u32 cnt;
} DMAChannelRegs;

void DMAStartTransferFB(u32 channel, u32 src, u32 dst, u32 cnt)
{
    volatile DMAChannelRegs *reg;
    u32 savedState;

    savedState = _ZN3IRQ7DisableEv();

    reg = (volatile DMAChannelRegs *)REG_DMA_BASE + channel;
    reg->src = src;
    reg->dst = dst;
    reg->cnt = cnt;

    (void)REG_DMA_BASE[0];
    (void)REG_DMA_BASE[0];

    _ZN3IRQ7RestoreEj(savedState);
}
