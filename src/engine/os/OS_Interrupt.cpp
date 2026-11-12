//cpp
/* The interrupt table and its masks (the Nitro SDK's os_interrupt.c), under
 * pret/pokediamond's OS_interrupt.c names.
 * Owns arm9 .text 0x02056d64..0x02057014: the table init, the handler
 * setter and getter, the DMA and timer callback hooks and the IE/IF mask
 * setters. mwccarm emits .text in reverse source order, so the file reads
 * top-down from OS_InitIrqTable at 0x02057000 to OS_ResetRequestIrqMask at
 * 0x02056d64.
 *
 * OS_IRQTable is the handler table, one entry per IE bit.
 * The four DMA channels (IE bits 8-11) and the four timers (IE bits 3-6) are
 * not handled through it: their handlers live in the callback table
 * OSi_IrqCallbackInfo (DMA 0-3, then timers 0-3), which the stubs in os_irqTable.c
 * (src/engine/os/OS_IrqTable.cpp) dispatch.
 *
 * PC port: smoke_gx builds OSi_EnterDmaCallback from this file through
 * port/tools/hostgen.py, which emits the whole file. port/ntr/runtime.cpp
 * supplies the host's interrupt registry (OS_EnableIrqMask and the rest), so
 * everything except OSi_EnterDmaCallback sits under #ifndef _MSC_VER.
 */

#include "nitro/os/interrupt.h"
#include "nitro/os/thread.h"
#include "nitro/hw/registers.h"

// The DTCM queue keeps C linkage: the PC port links this file, and MSVC
// mangles C++ data names. The tables are in nitro/os/interrupt.h.
extern "C" {
extern OSThreadQueue OSi_IrqThreadQueue; /* os_irqHandler.c, in DTCM */
}

#ifndef _MSC_VER

// OS_InitThreadQueue(&OSi_IrqThreadQueue): this SDK's thread queue is a u16
// mask of thread ids, so emptying it is a store of 0.
extern "C" void OS_InitIrqTable(void) {
    OSi_IrqThreadQueue = 0;
}

extern "C" void OS_SetIrqFunction(OSIrqMask intrBit, OSIrqFunction function) {
    for (int i = 0; i < OS_IRQ_TABLE_MAX; i++) {
        if (intrBit & 1) {
            OSIrqCallbackInfo *info = 0;
            if (REG_OS_IE_D0_SHIFT <= i && i <= REG_OS_IE_D3_SHIFT)
                info = &OSi_IrqCallbackInfo[i - REG_OS_IE_D0_SHIFT];
            else if (REG_OS_IE_T0_SHIFT <= i && i <= REG_OS_IE_T3_SHIFT)
                info = &OSi_IrqCallbackInfo[i - REG_OS_IE_T0_SHIFT + OSi_IRQCALLBACK_NO_TIMER0];
            else
                OS_IRQTable[i] = function;

            if (info) {
                info->func = (void (*)(void *))function;
                info->arg = 0;
                info->enable = 1;
            }
        }
        intrBit >>= 1;
    }
}

extern "C" OSIrqFunction OS_GetIrqFunction(OSIrqMask intrBit) {
    int i = 0;
    OSIrqFunction *funcPtr = &OS_IRQTable[0];
    for (; i < OS_IRQ_TABLE_MAX; i++) {
        if (intrBit & 1) {
            if (REG_OS_IE_D0_SHIFT <= i && i <= REG_OS_IE_D3_SHIFT)
                return (OSIrqFunction)OSi_IrqCallbackInfo[i - REG_OS_IE_D0_SHIFT].func;
            if (REG_OS_IE_T0_SHIFT <= i && i <= REG_OS_IE_T3_SHIFT)
                return (OSIrqFunction)
                    OSi_IrqCallbackInfo[i - REG_OS_IE_T0_SHIFT + OSi_IRQCALLBACK_NO_TIMER0]
                        .func;
            return *funcPtr;
        }
        intrBit >>= 1;
        funcPtr++;
    }
    return 0;
}

#endif

// Installs a DMA channel's completion callback and enables its interrupt.
// The entry stays armed only if the channel's interrupt was already enabled.
extern "C" void OSi_EnterDmaCallback(u32 dmaNo, void (*callback)(void *), void *arg) {
    OSIrqMask imask = 1UL << (REG_OS_IE_D0_SHIFT + dmaNo);

    OSi_IrqCallbackInfo[dmaNo].func = callback;
    OSi_IrqCallbackInfo[dmaNo].arg = arg;
    OSi_IrqCallbackInfo[dmaNo].enable = OS_EnableIrqMask(imask) & imask;
}

#ifndef _MSC_VER

// Installs a timer's overflow callback and enables its interrupt. The SDK
// indexes the entry as OSi_IrqCallbackInfo[timerNo + OSi_IRQCALLBACK_NO_TIMER0];
// the ROM's three literals (table + 0x30 + field) are what this compiler
// emits only when the offset is applied to the table before the index.
extern "C" void OSi_EnterTimerCallback(u32 timerNo, void (*callback)(void *), void *arg) {
    OSIrqMask imask = 1UL << (REG_OS_IE_T0_SHIFT + timerNo);

    (OSi_IrqCallbackInfo + OSi_IRQCALLBACK_NO_TIMER0)[timerNo].func = callback;
    (OSi_IrqCallbackInfo + OSi_IRQCALLBACK_NO_TIMER0)[timerNo].arg = arg;

    OS_EnableIrqMask(imask);
    (OSi_IrqCallbackInfo + OSi_IRQCALLBACK_NO_TIMER0)[timerNo].enable = 1;
}

extern "C" OSIrqMask OS_SetIrqMask(OSIrqMask mask) {
    u16 ime = REG_IME;
    REG_IME = 0;
    OSIrqMask old = REG_IE;
    REG_IE = mask;
    (void)REG_IME;
    REG_IME = ime;
    return old;
}

extern "C" OSIrqMask OS_EnableIrqMask(OSIrqMask mask) {
    u16 ime = REG_IME;
    REG_IME = 0;
    OSIrqMask old = REG_IE;
    REG_IE = old | mask;
    (void)REG_IME;
    REG_IME = ime;
    return old;
}

extern "C" OSIrqMask OS_DisableIrqMask(OSIrqMask mask) {
    u16 ime = REG_IME;
    REG_IME = 0;
    OSIrqMask old = REG_IE;
    REG_IE = old & ~mask;
    (void)REG_IME;
    REG_IME = ime;
    return old;
}

// Acknowledges the interrupts in mask and returns the requests that were
// pending before.
extern "C" OSIrqMask OS_ResetRequestIrqMask(OSIrqMask mask) {
    u16 ime = REG_IME;
    REG_IME = 0;
    OSIrqMask old = REG_IF;
    REG_IF = mask;
    (void)REG_IME;
    REG_IME = ime;
    return old;
}

#endif
