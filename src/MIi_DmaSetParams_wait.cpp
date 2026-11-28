//cpp
#include "types.h"
#include "nitro/hw/registers.h"
#include "nitro/mi/dma.h"
#include "nitro/os/system.h"

extern "C" {

#define DMA_CONTROL_DEST_FIXED (2u << 21)
#define GXFIFO_DMA_CONTROL (DMA_CONTROL_ENABLE | DMA_CONTROL_32_BIT | DMA_CONTROL_DEST_FIXED)

void MIi_DmaSetParams(u32 channel, u32 src, u32 dest, u32 ctrl);

void MI_SendGXCommand(u32 dmaNo, const void *src, u32 commandLength) {
    volatile u32 *dmaCtrl;
    u32 currentSrc;
    u32 chunk;

    if (commandLength == 0)
        return;

    dmaCtrl = REG_DMA_CNT_PTR(dmaNo);

    while (*dmaCtrl & DMA_CONTROL_ENABLE)
        ;

    currentSrc = (u32)src;
    while (commandLength != 0) {
        chunk = commandLength > 0x1d8 ? 0x1d8 : commandLength;
        MIi_DmaSetParams(dmaNo, currentSrc, (u32)&REG_GXFIFO, GXFIFO_DMA_CONTROL | (chunk >> 2));
        commandLength -= chunk;
        currentSrc += chunk;
    }

    while (*dmaCtrl & DMA_CONTROL_ENABLE)
        ;
}

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

typedef struct DMAChannelRegsWait {
    u32 src;
    u32 dst;
    u32 cnt;
} DMAChannelRegsWait;

void MIi_DmaSetParams_wait(u32 channel, u32 src, u32 dst, u32 cnt) {
    volatile DMAChannelRegsWait *reg;
    OSIntrMode savedState;

    savedState = OS_DisableInterrupts();

    reg = (DMAChannelRegsWait *)REG_DMA_BASE + channel;
    reg->src = src;
    reg->dst = dst;
    reg->cnt = cnt;

    REG_DMA_BASE[0];
    REG_DMA_BASE[0];

    OS_RestoreInterrupts(savedState);
}
}
