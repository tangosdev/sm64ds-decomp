#ifndef NITRO_OS_INTERRUPT_H
#define NITRO_OS_INTERRUPT_H
#ifndef __cplusplus
#define OS_ResetRequestIrqMask _Z22OS_ResetRequestIrqMaskj
#define OS_DisableIrqMask _Z17OS_DisableIrqMaskj
#define OS_EnableIrqMask _Z16OS_EnableIrqMaskj
#define OS_SetIrqMask _Z13OS_SetIrqMaskj
#define OSi_EnterTimerCallback _Z22OSi_EnterTimerCallbackjPFvPvES_
#define OSi_EnterDmaCallback _Z20OSi_EnterDmaCallbackjPFvPvES_
#define OS_GetIrqFunction _Z17OS_GetIrqFunctionj
#define OS_SetIrqFunction _Z17OS_SetIrqFunctionjPFvvE
#define OS_InitIrqTable _Z15OS_InitIrqTablev
#define OSi_IrqTimer3 _Z13OSi_IrqTimer3v
#define OSi_IrqTimer2 _Z13OSi_IrqTimer2v
#define OSi_IrqTimer1 _Z13OSi_IrqTimer1v
#define OSi_IrqTimer0 _Z13OSi_IrqTimer0v
#define OSi_IrqDma3 _Z11OSi_IrqDma3v
#define OSi_IrqDma2 _Z11OSi_IrqDma2v
#define OSi_IrqDma1 _Z11OSi_IrqDma1v
#define OSi_IrqDma0 _Z11OSi_IrqDma0v
#define OSi_IrqCallback _Z15OSi_IrqCallbacki
#define OS_IrqDummy _Z11OS_IrqDummyv
#endif

/*
 * The interrupt table and masks of the Nitro SDK, under the SDK's own names
 * (pret/pokediamond arm9/lib/NitroSDK/include/OS_interrupt.h,
 * OS_irqTable.h and OS_irqHandler.h). The bodies are os_interrupt.c
 * (src/engine/os/OS_Interrupt.cpp), os_irqTable.c
 * (src/engine/os/OS_IrqTable.cpp) and the ITCM half of os_irqHandler.c
 * (src/IRQHandler.cpp).
 *
 * An OSIrqMask is a set of IE bits. The four DMA channels (IE bits 8-11) and
 * the four timers (IE bits 3-6) are not served from the handler table: their
 * one-shot callbacks are OSIrqCallbackInfo entries, which the OSi_IrqDma* and
 * OSi_IrqTimer* stubs hand to OSi_IrqCallback.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"


#if defined(_MSC_VER) && defined(__cplusplus)
extern "C" {
#endif
typedef u32 OSIrqMask;

/* The IE/IF bits (os/common/interrupt.h). Macros, not enumerators or
 * inlines, so they cost an including file no @NNN numbers. */
#define OS_IE_V_BLANK (1UL << 0)
#define OS_IE_H_BLANK (1UL << 1)
#define OS_IE_V_COUNT (1UL << 2)
#define OS_IE_TIMER0 (1UL << 3)
#define OS_IE_TIMER1 (1UL << 4)
#define OS_IE_TIMER2 (1UL << 5)
#define OS_IE_TIMER3 (1UL << 6)
#define OS_IE_SIO (1UL << 7)
#define OS_IE_DMA0 (1UL << 8)
#define OS_IE_DMA1 (1UL << 9)
#define OS_IE_DMA2 (1UL << 10)
#define OS_IE_DMA3 (1UL << 11)
#define OS_IE_KEY (1UL << 12)
#define OS_IE_CARTRIDGE (1UL << 13)
#define OS_IE_SUBP (1UL << 16)
#define OS_IE_SPFIFO_SEND (1UL << 17)
#define OS_IE_SPFIFO_RECV (1UL << 18)
#define OS_IE_CARD_DATA (1UL << 19)
#define OS_IE_CARD_IREQ (1UL << 20)
#define OS_IE_GXFIFO (1UL << 21)

/* OS_IRQTable has one handler per IE bit; the DMA channels and the timers
 * dispatch through OSi_IrqCallbackInfo instead, at these indices. */
#define OS_IRQ_TABLE_MAX 22

#define OSi_IRQCALLBACK_NO_DMA0   0
#define OSi_IRQCALLBACK_NO_DMA1   1
#define OSi_IRQCALLBACK_NO_DMA2   2
#define OSi_IRQCALLBACK_NO_DMA3   3
#define OSi_IRQCALLBACK_NO_TIMER0 4
#define OSi_IRQCALLBACK_NO_TIMER1 5
#define OSi_IRQCALLBACK_NO_TIMER2 6
#define OSi_IRQCALLBACK_NO_TIMER3 7
#define OSi_IRQCALLBACK_NUM       (7 + 1)

typedef void (*OSIrqFunction)(void);

typedef struct OSIrqCallbackInfo {
    void (*func)(void *); /* 0x0 -- cleared once it has run */
    u32 enable;           /* 0x4 -- zero: mask the interrupt again after it */
    void *arg;            /* 0x8 */
} OSIrqCallbackInfo;

extern OSIrqFunction OS_IRQTable[];
extern OSIrqCallbackInfo OSi_IrqCallbackInfo[OSi_IRQCALLBACK_NUM];

/* os_interrupt.c */
void OS_InitIrqTable(void);
void OS_SetIrqFunction(OSIrqMask, OSIrqFunction);
OSIrqFunction OS_GetIrqFunction(OSIrqMask);
OSIrqMask OS_SetIrqMask(OSIrqMask);
OSIrqMask OS_EnableIrqMask(OSIrqMask);
OSIrqMask OS_DisableIrqMask(OSIrqMask);
OSIrqMask OS_ResetRequestIrqMask(OSIrqMask);
void OSi_EnterDmaCallback(u32, void (*)(void *), void *);
void OSi_EnterTimerCallback(u32, void (*)(void *), void *);

/* os_irqTable.c */
void OS_IrqDummy(void);
void OSi_IrqCallback(s32);
void OSi_IrqDma0(void);
void OSi_IrqDma1(void);
void OSi_IrqDma2(void);
void OSi_IrqDma3(void);
void OSi_IrqTimer0(void);
void OSi_IrqTimer1(void);
void OSi_IrqTimer2(void);
void OSi_IrqTimer3(void);

#ifdef __cplusplus
extern "C"
#endif
/* os_irqHandler.c */
void OS_IrqHandler(void);
#if defined(_MSC_VER) && defined(__cplusplus)
}
#endif


#endif /* NITRO_OS_INTERRUPT_H */
