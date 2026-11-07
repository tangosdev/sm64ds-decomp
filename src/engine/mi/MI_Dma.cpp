//cpp
/* DMA transfers (the Nitro SDK's mi_dma.c), arm9 .text
 * 0x02059f58..0x0205a21c.
 *
 * pret's pokediamond MI_dma.c defines, after the MIi_DmaSetParams family,
 * MI_DmaFill32, MI_DmaCopy32, MI_DmaCopy16, MI_DmaFill32Async,
 * MI_DmaCopy32Async, MI_WaitDma and MI_StopDma in this order, with the
 * functions this game never calls left out. mwccarm emits the bodies in
 * reverse, so MI_StopDma is lowest and MI_DmaFill32 highest.
 *
 * The SetParams functions live in ITCM: DMAStartTransfer (0x01ffde50) is
 * MIi_DmaSetParams and DMAStartTransferFB (0x01ffde00) is
 * MIi_DmaSetParams_wait_noInt. The synchronous copies keep their older
 * names here (DMASyncFillTransfer is MI_DmaFill32, DMASyncWordTransfer
 * MI_DmaCopy32, DMASyncHalfTransfer MI_DmaCopy16).
 *
 * This SDK is older than pret's: MI_WaitDma only spins on the enable bit,
 * and MI_StopDma has no DMA0 workaround.
 */
#include "types.h"
#include "nitro/hw/registers.h"
#include "IRQ.h"

typedef void (*MIDmaCallback)(void *);

#define MI_CNT_SRC_FIX DMA_CONTROL_SRC_FIXED
#define MI_CNT_32BIT DMA_CONTROL_32_BIT
#define MI_CNT_IRQ (1u << 30)
#define MI_CNT_ENABLE DMA_CONTROL_ENABLE

extern "C" {

/* MIi_DmaSetParams and MIi_DmaSetParams_wait_noInt, in ITCM */
extern void DMAStartTransfer(u32 dmaNo, u32 src, u32 dest, u32 ctrl);
extern void DMAStartTransferFB(u32 dmaNo, u32 src, u32 dest, u32 ctrl);
extern void OSi_EnterDmaCallback(u32 dmaNo, u32 callback, u32 arg);

void MI_WaitDma(u32 dmaNo);

/* MI_DmaFill32 */
void DMASyncFillTransfer(u32 dmaNo, void *dest, u32 data, u32 size) {
    vu32 *dmaCntp;
    vu32 *fillReg;

    if (size == 0)
        return;

    dmaCntp = REG_DMA_CNT_PTR(dmaNo);
    while (*dmaCntp & MI_CNT_ENABLE)
        ;

    fillReg = (vu32 *)REG_DMA_FILL_ADDR(dmaNo);
    *fillReg = data;
    DMAStartTransferFB(dmaNo, (u32)fillReg, (u32)dest,
                       (size >> 2) | MI_CNT_ENABLE | MI_CNT_32BIT | MI_CNT_SRC_FIX);

    while (*dmaCntp & MI_CNT_ENABLE)
        ;
}

/* MI_DmaCopy32 */
void DMASyncWordTransfer(u32 dmaNo, const void *src, void *dest, u32 size) {
    vu32 *dmaCntp;

    if (size == 0)
        return;

    dmaCntp = REG_DMA_CNT_PTR(dmaNo);
    while (*dmaCntp & MI_CNT_ENABLE)
        ;

    DMAStartTransferFB(dmaNo, (u32)src, (u32)dest, (size >> 2) | MI_CNT_ENABLE | MI_CNT_32BIT);

    while (*dmaCntp & MI_CNT_ENABLE)
        ;
}

/* MI_DmaCopy16 */
void DMASyncHalfTransfer(u32 dmaNo, const void *src, void *dest, u32 size) {
    vu32 *dmaCntp;

    if (size == 0)
        return;

    dmaCntp = REG_DMA_CNT_PTR(dmaNo);
    while (*dmaCntp & MI_CNT_ENABLE)
        ;

    DMAStartTransferFB(dmaNo, (u32)src, (u32)dest, (size >> 1) | MI_CNT_ENABLE);

    while (*dmaCntp & MI_CNT_ENABLE)
        ;
}

void MI_DmaFill32Async(u32 dmaNo, void *dest, u32 data, u32 size, MIDmaCallback callback,
                       void *arg) {
    if (size == 0) {
        if (callback)
            callback(arg);
        return;
    }

    MI_WaitDma(dmaNo);
    if (callback) {
        OSi_EnterDmaCallback(dmaNo, (u32)callback, (u32)arg);
        vu32 *fillReg = (vu32 *)REG_DMA_FILL_ADDR(dmaNo);
        *fillReg = data;
        DMAStartTransfer(dmaNo, (u32)fillReg, (u32)dest,
                         (size >> 2) | MI_CNT_ENABLE | MI_CNT_IRQ | MI_CNT_32BIT | MI_CNT_SRC_FIX);
    } else {
        vu32 *fillReg = (vu32 *)REG_DMA_FILL_ADDR(dmaNo);
        *fillReg = data;
        DMAStartTransfer(dmaNo, (u32)fillReg, (u32)dest,
                         (size >> 2) | MI_CNT_ENABLE | MI_CNT_32BIT | MI_CNT_SRC_FIX);
    }
}

void MI_DmaCopy32Async(u32 dmaNo, const void *src, void *dest, u32 size, MIDmaCallback callback,
                       void *arg) {
    if (size == 0) {
        if (callback)
            callback(arg);
        return;
    }

    MI_WaitDma(dmaNo);
    if (callback) {
        OSi_EnterDmaCallback(dmaNo, (u32)callback, (u32)arg);
        DMAStartTransfer(dmaNo, (u32)src, (u32)dest,
                         (size >> 2) | MI_CNT_ENABLE | MI_CNT_IRQ | MI_CNT_32BIT);
    } else {
        DMAStartTransfer(dmaNo, (u32)src, (u32)dest, (size >> 2) | MI_CNT_ENABLE | MI_CNT_32BIT);
    }
}

void MI_WaitDma(u32 dmaNo) {
    vu32 *dmaCntp = REG_DMA_CNT_PTR(dmaNo);

    while (*dmaCntp & MI_CNT_ENABLE)
        ;
}

/* Clears the channel's timing and repeat bits, then its enable bit, and
 * reads the control register twice while the channel winds down. */
void MI_StopDma(u32 dmaNo) {
    u32 enabled = IRQ::Disable();
    vu16 *dmaCntp = &((vu16 *)REG_DMA_BASE)[dmaNo * 6 + 5];

    *dmaCntp &= ~0x3a00;
    *dmaCntp &= ~0x8000;
    (void)*dmaCntp;
    (void)*dmaCntp;

    IRQ::Restore(enabled);
}
}
