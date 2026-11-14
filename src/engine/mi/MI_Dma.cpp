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
 * The SetParams functions live in ITCM: MIi_DmaSetParams (0x01ffde50) and
 * MIi_DmaSetParams_wait (0x01ffde00), which the synchronous copies call; both
 * mask interrupts, and _wait adds the two dummy DMA0SAD reads.
 *
 * This SDK is older than pret's: MI_WaitDma only spins on the enable bit,
 * and MI_StopDma has no DMA0 workaround.
 */
#include "types.h"
#include "nitro/hw/registers.h"
#include "nitro/os/system.h"
#include "nitro/os/interrupt.h"
#include "nitro/mi/dma.h"
/* The ITCM body is C++ (_Z21MIi_DmaSetParams_waitjjjj). The host stub is C. */
#if defined(_MSC_VER)
extern "C"
#endif
    void MIi_DmaSetParams_wait(u32, u32, u32, u32);

#define MI_CNT_SRC_FIX DMA_CONTROL_SRC_FIXED
#define MI_CNT_32BIT DMA_CONTROL_32_BIT
#define MI_CNT_IRQ (1u << 30)
#define MI_CNT_ENABLE DMA_CONTROL_ENABLE

extern "C" {

/* in ITCM */
extern void MIi_DmaSetParams(u32 dmaNo, u32 src, u32 dest, u32 ctrl);
}

void MI_DmaFill32(u32 dmaNo, void *dest, u32 data, u32 size) {
    vu32 *dmaCntp;
    vu32 *fillReg;

    if (size == 0)
        return;

    dmaCntp = REG_DMA_CNT_PTR(dmaNo);
    while (*dmaCntp & MI_CNT_ENABLE)
        ;

    fillReg = (vu32 *)REG_DMA_FILL_ADDR(dmaNo);
    *fillReg = data;
    MIi_DmaSetParams_wait(dmaNo, (u32)fillReg, (u32)dest,
                          (size >> 2) | MI_CNT_ENABLE | MI_CNT_32BIT | MI_CNT_SRC_FIX);

    while (*dmaCntp & MI_CNT_ENABLE)
        ;
}

void MI_DmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size) {
    vu32 *dmaCntp;

    if (size == 0)
        return;

    dmaCntp = REG_DMA_CNT_PTR(dmaNo);
    while (*dmaCntp & MI_CNT_ENABLE)
        ;

    MIi_DmaSetParams_wait(dmaNo, (u32)src, (u32)dest, (size >> 2) | MI_CNT_ENABLE | MI_CNT_32BIT);

    while (*dmaCntp & MI_CNT_ENABLE)
        ;
}

void MI_DmaCopy16(u32 dmaNo, const void *src, void *dest, u32 size) {
    vu32 *dmaCntp;

    if (size == 0)
        return;

    dmaCntp = REG_DMA_CNT_PTR(dmaNo);
    while (*dmaCntp & MI_CNT_ENABLE)
        ;

    MIi_DmaSetParams_wait(dmaNo, (u32)src, (u32)dest, (size >> 1) | MI_CNT_ENABLE);

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
        OSi_EnterDmaCallback(dmaNo, callback, arg);
        vu32 *fillReg = (vu32 *)REG_DMA_FILL_ADDR(dmaNo);
        *fillReg = data;
        MIi_DmaSetParams(dmaNo, (u32)fillReg, (u32)dest,
                         (size >> 2) | MI_CNT_ENABLE | MI_CNT_IRQ | MI_CNT_32BIT | MI_CNT_SRC_FIX);
    } else {
        vu32 *fillReg = (vu32 *)REG_DMA_FILL_ADDR(dmaNo);
        *fillReg = data;
        MIi_DmaSetParams(dmaNo, (u32)fillReg, (u32)dest,
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
        OSi_EnterDmaCallback(dmaNo, callback, arg);
        MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest,
                         (size >> 2) | MI_CNT_ENABLE | MI_CNT_IRQ | MI_CNT_32BIT);
    } else {
        MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, (size >> 2) | MI_CNT_ENABLE | MI_CNT_32BIT);
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
    OSIntrMode enabled = OS_DisableInterrupts();
    vu16 *dmaCntp = &((vu16 *)REG_DMA_BASE)[dmaNo * 6 + 5];

    *dmaCntp &= ~0x3a00;
    *dmaCntp &= ~0x8000;
    (void)*dmaCntp;
    (void)*dmaCntp;

    OS_RestoreInterrupts(enabled);
}
