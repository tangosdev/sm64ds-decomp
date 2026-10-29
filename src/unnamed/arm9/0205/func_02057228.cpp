//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

typedef struct {
    u32 lock;
    u16 owner;
} LockObj;


s32 func_02057228(u32 callerID, LockObj *addr, void (*cleanupFn)(void), u32 useAll)
{
    u32 state;

    if (callerID != addr->owner)
        return ~1;

    if (useAll)
        state = IRQ::DisableAll();
    else
        state = IRQ::Disable();

    addr->owner = 0;
    if (cleanupFn)
        cleanupFn();
    addr->lock = 0;

    if (useAll)
        IRQ::RestoreAll(state);
    else
        IRQ::Restore(state);

    return 0;
}
}
