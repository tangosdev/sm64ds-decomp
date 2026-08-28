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

}

#endif
