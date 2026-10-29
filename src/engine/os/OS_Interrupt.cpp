//cpp
/* IRQ -- the interrupt table and its masks (the Nitro SDK's os_interrupt.c).
 * Owns arm9 .text 0x02056d64..0x02057014: the table init, the handler
 * setter and getter, the DMA and timer callback hooks and the IE/IF mask
 * setters. mwccarm emits .text in reverse source order, so the file reads
 * top-down from the table init at 0x02057000 to ClearInterrupts at
 * 0x02056d64.
 *
 * data_02099fe4 is the handler table, one entry per IE bit. The four DMA
 * channels (IE bits 8-11) and the four timers (IE bits 3-6) are not handled
 * through it: their handlers live in the callback table data_020a60c4
 * (DMA 0-3, then timers 0-3 from data_020a60f4), which the stubs in
 * os_irqTable.c (src/engine/os/OS_IrqTable.cpp) dispatch.
 *
 * PC port: smoke_gx builds func_02056e98 from this file through
 * port/tools/hostgen.py, which emits the whole file. port/ntr/runtime.cpp
 * supplies the host's interrupt registry (IRQ::EnableIRQs and the rest), so
 * everything except func_02056e98 sits under #ifndef _MSC_VER.
 */

#include "IRQ.h"
#include "nitro/hw/registers.h"

#define REG_IME (*(volatile u16 *)0x04000208)

/* C linkage: these are ROM data symbols named by their exact final names, so a
   C++ translation unit must not mangle them -- src/IRQ.cpp declared all of them
   inside its extern "C" block before this file was split out of it. */
extern "C" {
extern IRQ::Handler data_02099fe4[];
extern IRQ::HandlerEntry data_020a60c4[];
extern IRQ::HandlerEntry data_020a60f4[];
extern short data_023c0000[];

void func_02057000(void);
void func_02056e98(u32 idx, u32 handler, u32 arg);
void func_02056e4c(u32 idx, u32 handler, u32 arg);
}

#ifndef _MSC_VER

// Clears the halfword at the start of DTCM that the IRQ wait loop reads.
extern "C" void func_02057000(void) {
    data_023c0000[0] = 0;
}

namespace IRQ {

void SetIRQHandler(u32 mask, Handler handler) {
    for (int i = 0; i < 0x16; i++) {
        if (mask & 1) {
            HandlerEntry *entry = 0;
            if (i >= 8 && i <= 0xb)
                entry = &data_020a60c4[i - 8];
            else if (i >= 3 && i <= 6)
                entry = &data_020a60c4[i + 1];
            else
                data_02099fe4[i] = handler;

            if (entry) {
                entry->handler = handler;
                entry->argument = 0;
                entry->enabled = 1;
            }
        }
        mask >>= 1;
    }
}

Handler GetIRQHandler(u32 mask) {
    int i = 0;
    Handler *p = data_02099fe4;
    for (; i < 0x16; i++) {
        if (mask & 1) {
            if (i >= 8 && i <= 0xb)
                return data_020a60c4[i - 8].handler;
            if (i >= 3 && i <= 6)
                return data_020a60c4[i + 1].handler;
            return *p;
        }
        mask >>= 1;
        p++;
    }
    return 0;
}

} // namespace IRQ

#endif

// Installs a DMA channel's completion callback and enables its interrupt.
// The entry stays armed only if the channel's interrupt was already enabled.
extern "C" void func_02056e98(u32 idx, u32 handler, u32 arg) {
    u32 mask = 1u << (idx + 8);
    data_020a60c4[idx].handler = (IRQ::Handler)handler;
    data_020a60c4[idx].argument = arg;
    u32 prev_ie = IRQ::EnableIRQs(mask);
    data_020a60c4[idx].enabled = prev_ie & (1u << (idx + 8));
}

#ifndef _MSC_VER

// Installs a timer's overflow callback and enables its interrupt.
extern "C" void func_02056e4c(u32 idx, u32 handler, u32 arg) {
    data_020a60f4[idx].handler = (IRQ::Handler)handler;
    data_020a60f4[idx].argument = arg;
    IRQ::EnableIRQs(1u << (idx + 3));
    data_020a60f4[idx].enabled = 1;
}

namespace IRQ {

u32 SetIRQs(u32 mask) {
    u16 ime = REG_IME;
    REG_IME = 0;
    u32 old = REG_IE;
    REG_IE = mask;
    (void)REG_IME;
    REG_IME = ime;
    return old;
}

u32 EnableIRQs(u32 mask) {
    u16 ime = REG_IME;
    REG_IME = 0;
    u32 old = REG_IE;
    REG_IE = old | mask;
    (void)REG_IME;
    REG_IME = ime;
    return old;
}

u32 DisableIRQs(u32 mask) {
    u16 ime = REG_IME;
    REG_IME = 0;
    u32 old = REG_IE;
    REG_IE = old & ~mask;
    (void)REG_IME;
    REG_IME = ime;
    return old;
}

// Acknowledges the interrupts in mask and returns the requests that were
// pending before.
u32 ClearInterrupts(u32 mask) {
    u16 ime = REG_IME;
    REG_IME = 0;
    u32 old = REG_IF;
    REG_IF = mask;
    (void)REG_IME;
    REG_IME = ime;
    return old;
}

} // namespace IRQ

#endif
