//cpp
/* MIi_DmaSetParams at 0x01ffde50 (ITCM)
 *
 * Arms one DMA channel: writes its source, destination and control registers
 * with interrupts masked, so the three-word program cannot be torn by an IRQ
 * that touches the same channel. Writing the control word last is what starts
 * the transfer.
 *
 * The channel's register block is three consecutive words at
 * REG_DMA_BASE + channel * 12; the twin MIi_DmaSetParams_wait at 0x01ffde00
 * is the same function plus two dummy reads of REG_DMA_BASE after the
 * stores, and indexes the channel block the same way.
 */

#include "nitro/hw/registers.h"
#include "nitro/mi/dma.h"
#include "nitro/os/system.h"

extern "C" {

/* Source, destination and control for one DMA channel. */
typedef struct DMAChannelRegs {
    vu32 src;
    vu32 dest;
    vu32 ctrl;
} DMAChannelRegs;

void MIi_DmaSetParams(u32 channel, u32 src, u32 dest, u32 ctrl) {
    OSIntrMode state = OS_DisableInterrupts();
    DMAChannelRegs *reg = (DMAChannelRegs *)REG_DMA_BASE + channel;

    reg->src = src;
    reg->dest = dest;
    reg->ctrl = ctrl;

    OS_RestoreInterrupts(state);
}
}
