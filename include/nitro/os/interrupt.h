#ifndef NITRO_OS_INTERRUPT_H
#define NITRO_OS_INTERRUPT_H

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

#ifdef __cplusplus
extern "C" {
#endif

typedef u32 OSIrqMask;

typedef void (*OSIrqFunction)(void);

typedef struct OSIrqCallbackInfo {
    void (*func)(void *); /* 0x0 -- cleared once it has run */
    u32 enable;           /* 0x4 -- zero: mask the interrupt again after it */
    void *arg;            /* 0x8 */
} OSIrqCallbackInfo;

/* os_interrupt.c */
void OS_InitIrqTable(void);
void OS_SetIrqFunction(OSIrqMask, OSIrqFunction);
OSIrqFunction OS_GetIrqFunction(OSIrqMask);
OSIrqMask OS_SetIrqMask(OSIrqMask);
OSIrqMask OS_EnableIrqMask(OSIrqMask);
OSIrqMask OS_DisableIrqMask(OSIrqMask);
OSIrqMask OS_ResetRequestIrqMask(OSIrqMask);

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

/* os_irqHandler.c */
void OS_IrqHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_OS_INTERRUPT_H */
