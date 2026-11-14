//cpp
/* Asynchronous geometry-command DMA (the Nitro SDK's mi_dma_gxcommand.c), arm9
 * .text 0x0205a21c..0x0205a448.
 *
 * MI_SendGXCommandAsync feeds a command list to the geometry FIFO in chunks
 * of at most MIi_GX_LENGTH_ONCE bytes. While it runs, it takes over the
 * GXFIFO interrupt: MIi_FIFOCallback starts the next chunk each time the FIFO
 * drops below half full. The last chunk raises the DMA interrupt instead, and
 * MIi_DMACallback then puts the FIFO interrupt back and runs the caller's
 * callback.
 *
 * pret's pokediamond MI_dma_gxcommand.c defines MI_SendGXCommandAsync,
 * MIi_FIFOCallback and MIi_DMACallback in this order (after
 * MI_SendGXCommand, which goes to ITCM: src/MI_SendGXCommand.c). mwccarm
 * emits the bodies in reverse, so MIi_DMACallback is lowest. This SDK has no
 * MIi_CheckDma0SourceAddress, and MI_SendGXCommandAsyncFast is not in this
 * ROM.
 */
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/os/interrupt.h"
#include "nitro/mi/dma.h"
#include "nitro/hw/registers.h"

#define MIi_GX_LENGTH_ONCE 0x1d8
#define REG_GXFIFO_ADDR 0x04000400

/* MI_CNT_SEND32: enable, 32-bit, destination fixed, start immediately */
#define MI_CNT_SEND32(size) (0x84400000 | ((size) >> 2))
#define MI_CNT_SEND32_IF(size) (0xc4400000 | ((size) >> 2))

typedef struct MIiGXDmaParams {
    int isBusy; /* not volatile in this SDK: the busy wait reads it once */
    u32 dmaNo;
    u32 src;
    u32 length;
    MIDmaCallback callback;
    void *arg;
    u32 fifoCond;
    OSIrqFunction fifoFunc;
} MIiGXDmaParams;

static inline void MIi_CallCallback(MIDmaCallback callback, void *arg) {
    if (callback)
        callback(arg);
}

/* BSS word. mwcc leaves the name unmangled. The host stub is C. */
#if defined(_MSC_VER)
extern "C"
#endif
    extern MIiGXDmaParams MIi_GXDmaParams;
extern "C" {

/* in ITCM */
extern void MIi_DmaSetParams(u32 dmaNo, u32 src, u32 dest, u32 ctrl);
}

void MIi_FIFOCallback(void);
void MIi_DMACallback(void *arg);

void MI_SendGXCommandAsync(u32 dmaNo, const void *src, u32 commandLength, MIDmaCallback callback,
                           void *arg) {
    if (!commandLength) {
        MIi_CallCallback(callback, arg);
        return;
    }

    while (MIi_GXDmaParams.isBusy) {
    }

    /* G3X_GetCommandFifoStatus() & GX_FIFOSTAT_UNDERHALF */
    while (!((REG_GXSTAT & 0x7000000) >> 24 & 2)) {
    }

    MIi_GXDmaParams.isBusy = 1;
    MIi_GXDmaParams.dmaNo = dmaNo;
    MIi_GXDmaParams.src = (u32)src;
    MIi_GXDmaParams.length = commandLength;
    MIi_GXDmaParams.callback = callback;
    MIi_GXDmaParams.arg = arg;

    MI_WaitDma(dmaNo);
    {
        OSIntrMode lastIntrMode = OS_DisableInterrupts();

        MIi_GXDmaParams.fifoCond = (REG_GXSTAT & 0xc0000000) >> 30;
        MIi_GXDmaParams.fifoFunc = OS_GetIrqFunction(OS_IE_GXFIFO);

        /* G3X_SetFifoIntrCond(GX_FIFOINTR_COND_UNDERHALF) */
        REG_GXSTAT = (REG_GXSTAT & ~0xc0000000) | 0x40000000;
        OS_SetIrqFunction(OS_IE_GXFIFO, MIi_FIFOCallback);
        OS_EnableIrqMask(OS_IE_GXFIFO);

        MIi_FIFOCallback();

        OS_RestoreInterrupts(lastIntrMode);
    }
}

void MIi_FIFOCallback(void) {
    if (!MIi_GXDmaParams.length)
        return;

    u32 length = (MIi_GXDmaParams.length >= MIi_GX_LENGTH_ONCE) ? MIi_GX_LENGTH_ONCE
                                                                : MIi_GXDmaParams.length;
    u32 src = MIi_GXDmaParams.src;

    MIi_GXDmaParams.length -= length;
    MIi_GXDmaParams.src += length;

    if (!MIi_GXDmaParams.length) {
        OSi_EnterDmaCallback(MIi_GXDmaParams.dmaNo, MIi_DMACallback, 0);
        MIi_DmaSetParams(MIi_GXDmaParams.dmaNo, src, REG_GXFIFO_ADDR, MI_CNT_SEND32_IF(length));
        OS_ResetRequestIrqMask(OS_IE_GXFIFO);
    } else {
        MIi_DmaSetParams(MIi_GXDmaParams.dmaNo, src, REG_GXFIFO_ADDR, MI_CNT_SEND32(length));
        OS_ResetRequestIrqMask(OS_IE_GXFIFO);
    }
}

void MIi_DMACallback(void *arg) {
    OS_DisableIrqMask(OS_IE_GXFIFO);

    /* G3X_SetFifoIntrCond(MIi_GXDmaParams.fifoCond) */
    REG_GXSTAT = (MIi_GXDmaParams.fifoCond << 30) | (REG_GXSTAT & ~0xc0000000);
    OS_SetIrqFunction(OS_IE_GXFIFO, MIi_GXDmaParams.fifoFunc);

    MIi_GXDmaParams.isBusy = 0;

    MIi_CallCallback(MIi_GXDmaParams.callback, MIi_GXDmaParams.arg);
}
