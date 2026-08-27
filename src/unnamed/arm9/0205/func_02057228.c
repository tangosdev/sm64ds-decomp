#include "types.h"
typedef struct {
    u32 lock;
    u16 owner;
} LockObj;

extern u32 _ZN3IRQ10DisableAllEv(void);
extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ10RestoreAllEj(u32 state);
extern void _ZN3IRQ7RestoreEj(u32 state);

s32 func_02057228(u32 callerID, LockObj *addr, void (*cleanupFn)(void), u32 useAll)
{
    u32 state;

    if (callerID != addr->owner)
        return ~1;

    if (useAll)
        state = _ZN3IRQ10DisableAllEv();
    else
        state = _ZN3IRQ7DisableEv();

    addr->owner = 0;
    if (cleanupFn)
        cleanupFn();
    addr->lock = 0;

    if (useAll)
        _ZN3IRQ10RestoreAllEj(state);
    else
        _ZN3IRQ7RestoreEj(state);

    return 0;
}
