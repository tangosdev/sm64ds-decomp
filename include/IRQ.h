#ifndef IRQ_H
#define IRQ_H

#include "types.h"

namespace IRQ {

typedef void (*Handler)(void);

struct HandlerEntry {
    Handler handler;
    u32 enabled;
    u32 argument;
};

Handler GetIRQHandler(u32 mask);
void SetIRQHandler(u32 mask, Handler handler);
u32 EnableIRQs(u32 mask);
u32 DisableIRQs(u32 mask);
u32 SetIRQs(u32);
u32 ClearInterrupts(u32);

/* CPSR interrupt-disable primitives (src/IRQ3.cpp): each returns the previous
 * CPSR state for the matching Restore. Enable/Disable/Restore act on the IRQ
 * bit, DisableAll/RestoreAll on IRQ and FIQ together. */
u32 Enable();
u32 Disable();
u32 Restore(u32);
u32 DisableAll();
u32 RestoreAll(u32);

}

#endif
