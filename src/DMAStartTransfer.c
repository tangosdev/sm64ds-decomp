/* DMAStartTransfer at 0x01ffde50 (ITCM)
 *
 * Arms one DMA channel: writes its source, destination and control registers
 * with interrupts masked, so the three-word program cannot be torn by an IRQ
 * that touches the same channel. Writing the control word last is what starts
 * the transfer.
 *
 * The channel's register block is three consecutive words at
 * REG_DMA_BASE + channel * 12; the twin DMAStartTransferFB at 0x01ffde00 is the
 * same function plus two dummy reads of REG_DMA_BASE after the stores.
 *
 * The u64 mask is the project's usual launder to stop mwccarm splitting the
 * 0x040000b0 literal into `add #0x4000000` plus a #0xb0 store offset; it keeps
 * the base in one register so all three stores index off it (see
 * notes/mwccarm-codegen.md 6g, and DMASyncFillTransfer.c which does the same).
 */

#include "nitro/hw/registers.h"

/* Source, destination and control for one DMA channel. */
typedef struct DMAChannelRegs {
    vu32 src;
    vu32 dest;
    vu32 ctrl;
} DMAChannelRegs;

extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 state);

void DMAStartTransfer(u32 channel, u32 src, u32 dest, u32 ctrl)
{
    u32 state = _ZN3IRQ7DisableEv();
    DMAChannelRegs *reg = (DMAChannelRegs *)((u32)REG_DMA_BASE +
        channel * sizeof(DMAChannelRegs));

    reg->src = src;
    reg->dest = dest;
    reg->ctrl = ctrl;

    _ZN3IRQ7RestoreEj(state);
}
