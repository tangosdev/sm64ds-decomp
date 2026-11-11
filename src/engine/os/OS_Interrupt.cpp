//cpp
/* The interrupt table and its masks (the Nitro SDK's os_interrupt.c), under
 * pret/pokediamond's OS_interrupt.c names.
 * Owns arm9 .text 0x02056d64..0x02057014: the table init, the handler
 * setter and getter, the DMA and timer callback hooks and the IE/IF mask
 * setters. mwccarm emits .text in reverse source order, so the file reads
 * top-down from OS_InitIrqTable at 0x02057000 to OS_ResetRequestIrqMask at
 * 0x02056d64.
 *
 * data_02099fe4 is the handler table (OS_IRQTable), one entry per IE bit.
 * The four DMA channels (IE bits 8-11) and the four timers (IE bits 3-6) are
 * not handled through it: their handlers live in the callback table
 * data_020a60c4 (OSi_IrqCallbackInfo: DMA 0-3, then timers 0-3 from
 * data_020a60f4), which the stubs in os_irqTable.c
 * (src/engine/os/OS_IrqTable.cpp) dispatch.
 *
 * PC port: smoke_gx builds OSi_EnterDmaCallback from this file through
 * port/tools/hostgen.py, which emits the whole file. port/ntr/runtime.cpp
 * supplies the host's interrupt registry (OS_EnableIrqMask and the rest), so
 * everything except OSi_EnterDmaCallback sits under #ifndef _MSC_VER.
 */

#include "nitro/os/interrupt.h"
#include "nitro/hw/registers.h"

#define REG_IME (*(volatile u16 *)0x04000208)

// The tables keep C linkage: the PC port links this file, and MSVC
// mangles C++ data names.
extern "C" {
extern OSIrqFunction data_02099fe4[];
extern OSIrqCallbackInfo data_020a60c4[];
extern OSIrqCallbackInfo data_020a60f4[];
extern short data_023c0000[];

// local extern: the SDK's (u32, void (*)(void *), void *) parameters are not
// adopted yet; their callers in MI_Dma, MI_DmaGXCommand, OS_Alarm and OS_Tick
// still pass words.
void OSi_EnterDmaCallback(u32 idx, u32 handler, u32 arg);
void OSi_EnterTimerCallback(u32 idx, u32 handler, u32 arg);
}

#ifndef _MSC_VER

// OS_InitThreadQueue(&OSi_IrqThreadQueue): this SDK's thread queue is a u16
// mask of thread ids, and the IRQ thread queue is the first halfword of DTCM.
extern "C" void OS_InitIrqTable(void) {
    data_023c0000[0] = 0;
}

extern "C" void OS_SetIrqFunction(OSIrqMask intrBit, OSIrqFunction function) {
    for (int i = 0; i < 0x16; i++) {
        if (intrBit & 1) {
            OSIrqCallbackInfo *info = 0;
            if (i >= 8 && i <= 0xb)
                info = &data_020a60c4[i - 8];
            else if (i >= 3 && i <= 6)
                info = &data_020a60c4[i + 1];
            else
                data_02099fe4[i] = function;

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
    OSIrqFunction *funcPtr = data_02099fe4;
    for (; i < 0x16; i++) {
        if (intrBit & 1) {
            if (i >= 8 && i <= 0xb)
                return (OSIrqFunction)data_020a60c4[i - 8].func;
            if (i >= 3 && i <= 6)
                return (OSIrqFunction)data_020a60c4[i + 1].func;
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
extern "C" void OSi_EnterDmaCallback(u32 idx, u32 handler, u32 arg) {
    OSIrqMask mask = 1u << (idx + 8);
    data_020a60c4[idx].func = (void (*)(void *))handler;
    data_020a60c4[idx].arg = (void *)arg;
    OSIrqMask prev_ie = OS_EnableIrqMask(mask);
    data_020a60c4[idx].enable = prev_ie & (1u << (idx + 8));
}

#ifndef _MSC_VER

// Installs a timer's overflow callback and enables its interrupt.
extern "C" void OSi_EnterTimerCallback(u32 idx, u32 handler, u32 arg) {
    data_020a60f4[idx].func = (void (*)(void *))handler;
    data_020a60f4[idx].arg = (void *)arg;
    OS_EnableIrqMask(1u << (idx + 3));
    data_020a60f4[idx].enable = 1;
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
