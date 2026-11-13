//cpp
#include "types.h"
#include "nitro/os/spinLock.h"

namespace IRQ {
void SetFIQState(bool);
void SetIRQState(bool);
}

/* flags & 2 is 0 or 2. A bool parameter would turn 2 into 1, so the call
   goes through an int function pointer. The bl still targets SetFIQState. */
void func_0206ce90(u32 flags) {
    if (flags & 0x20) {
        OS_UnLockCartridge(0x70);
    }
    ((void (*)(int))IRQ::SetFIQState)(flags & 2);
    ((void (*)(int))IRQ::SetIRQState)(flags & 1);
}