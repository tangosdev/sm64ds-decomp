//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

typedef struct {
    u32 lock;
    u16 owner;
} LockObj;


extern u32 func_0205a74c(u32 val, LockObj *addr);

u32 func_02057198(u32 val, LockObj *addr, void (*cleanupFn)(void), u32 useAll)
{
    u32 state;
    u32 old;

    if (useAll)
        state = IRQ::DisableAll();
    else
        state = IRQ::Disable();

    old = func_0205a74c(val, addr);
    if (old == 0)
    {
        if (cleanupFn)
            cleanupFn();
        addr->owner = (u16)val;
    }

    if (useAll)
        IRQ::RestoreAll(state);
    else
        IRQ::Restore(state);

    return old;
}
}
