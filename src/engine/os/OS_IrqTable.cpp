//cpp
/* IRQ -- the DMA and timer interrupt stubs (the Nitro SDK's os_irqTable.c).
 * Owns arm9 .text 0x02056c40..0x02056d64: the empty handler, the shared
 * DMA/timer callback dispatch and its eight per-channel stubs. mwccarm emits
 * .text in reverse source order, so the file reads top-down from the empty
 * handler at 0x02056d60 to the timer-3 stub at 0x02056c40.
 *
 * The four DMA channels (IE bits 8-11) and the four timers (IE bits 3-6) are
 * not dispatched through the IE handler table: their callbacks live in
 * data_020a60c4 (DMA 0-3, then timers 0-3), which each stub hands to
 * DmaTimHandler by index. data_02099fd4 gives each callback entry its IE bit.
 * The masks and the table setters are os_interrupt.c
 * (src/engine/os/OS_Interrupt.cpp).
 */

#include "IRQ.h"

typedef void (*IrqCallback)(void *);

/* C linkage: these are ROM data symbols named by their exact final names, so a
   C++ translation unit must not mangle them -- src/IRQ.cpp declared all of them
   inside its extern "C" block before this file was split out of it. */
extern "C" {
extern IRQ::HandlerEntry data_020a60c4[];
extern u16 data_02099fd4[];
extern short data_023c0000[];
}

namespace IRQ {

// Installed for vectors that have no handler.
void EmptyHandler(void) {}

inline IrqCallback CallbackOf(HandlerEntry *table, u32 idx) {
    return (IrqCallback)table[idx].handler;
}

// Shared body of the DMA and timer stubs: runs the entry's one-shot
// callback, flags the interrupt as serviced for the IRQ wait loop, and
// masks the interrupt again unless the entry was installed already enabled.
void DmaTimHandler(u32 idx) {
    u16 bit = data_02099fd4[idx];
    u32 mask = 1u << bit;
    IrqCallback fn = CallbackOf(data_020a60c4, idx);
    data_020a60c4[idx].handler = 0;
    if (0 != fn) {
        fn((void *)data_020a60c4[idx].argument);
    }
    *(volatile u32 *)((int)data_023c0000 + 0x3ff8) |= mask;
    if (data_020a60c4[idx].enabled != 0) {
        return;
    }
    DisableIRQs(mask);
}

void Dma0Handler() {
    DmaTimHandler(0);
}

void Dma1Handler() {
    DmaTimHandler(1);
}

void Dma2Handler() {
    DmaTimHandler(2);
}

void Dma3Handler() {
    DmaTimHandler(3);
}

void Tim0OverflowHandler() {
    DmaTimHandler(4);
}

void Tim1OverflowHandler() {
    DmaTimHandler(5);
}

void Tim2OverflowHandler() {
    DmaTimHandler(6);
}

void Tim3OverflowHandler() {
    DmaTimHandler(7);
}

} // namespace IRQ
