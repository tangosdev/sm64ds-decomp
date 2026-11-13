//cpp
/* The DMA and timer interrupt stubs (the Nitro SDK's os_irqTable.c).
 * Owns arm9 .text 0x02056c40..0x02056d64: OS_IrqDummy, the shared DMA/timer
 * callback dispatch OSi_IrqCallback and its eight per-channel stubs, under
 * pret/pokediamond's OS_irqTable.c names. mwccarm emits .text in reverse
 * source order, so the file reads top-down from OS_IrqDummy at 0x02056d60 to
 * OSi_IrqTimer3 at 0x02056c40.
 *
 * The four DMA channels (IE bits 8-11) and the four timers (IE bits 3-6) are
 * not dispatched through the IE handler table: their callbacks live in
 * OSi_IrqCallbackInfo (DMA 0-3, then timers 0-3), which each stub hands to
 * OSi_IrqCallback by index. OSi_IrqCallbackInfoIndex gives each callback
 * entry its IE bit. The masks
 * and the table setters are os_interrupt.c (src/engine/os/OS_Interrupt.cpp).
 */

#include "nitro/os/interrupt.h"
#include "nitro/os/interrupt_check.h"


extern u16 OSi_IrqCallbackInfoIndex[OSi_IRQCALLBACK_NUM];
 /* DMA 0-3, then timers 0-3 */

// Installed for vectors that have no handler.
void OS_IrqDummy(void) {}

// Shared body of the DMA and timer stubs: runs the entry's one-shot
// callback, flags the interrupt as serviced for the IRQ wait loop, and
// masks the interrupt again unless the entry was installed already enabled.
void OSi_IrqCallback(s32 index) {
    OSIrqMask imask = 1UL << OSi_IrqCallbackInfoIndex[index];
    void (*callback)(void *) = OSi_IrqCallbackInfo[index].func;

    OSi_IrqCallbackInfo[index].func = 0;

    if (callback) {
        callback(OSi_IrqCallbackInfo[index].arg);
    }

    OS_SetIrqCheckFlag(imask);

    if (!OSi_IrqCallbackInfo[index].enable) {
        OS_DisableIrqMask(imask);
    }
}

void OSi_IrqDma0(void) {
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_DMA0);
}

void OSi_IrqDma1(void) {
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_DMA1);
}

void OSi_IrqDma2(void) {
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_DMA2);
}

void OSi_IrqDma3(void) {
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_DMA3);
}

void OSi_IrqTimer0(void) {
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_TIMER0);
}

void OSi_IrqTimer1(void) {
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_TIMER1);
}

void OSi_IrqTimer2(void) {
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_TIMER2);
}

void OSi_IrqTimer3(void) {
    OSi_IrqCallback(OSi_IRQCALLBACK_NO_TIMER3);
}
