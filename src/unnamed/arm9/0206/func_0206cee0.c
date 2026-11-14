//cpp
#include "types.h"
#include "nitro/os/spinLock.h"

namespace IRQ {
void SetFIQState(bool);
void SetIRQState(bool);
}

extern "C" {
extern int func_0206da18(void);
extern int func_0206d9cc(void);
/* One try at the cartridge lock, with IRQ and FIQ disabled first (the
 * func_0206da18 / func_0206d9cc entries return the previous states). Returns
 * nonzero if the lock was taken with lock id 0x70, or if the main processor
 * already owns it. On success *out gets the flags func_0206ce90 restores
 * from: 1 IRQ was on, 2 FIQ was on, 0x20 this call took the lock. On failure
 * the interrupt states are put back and *out is 0. func_0206cec0 spins on it.
 * No SDK or MSL function with this body has been found. */
int func_0206cee0(int *out) {
    int irq = func_0206da18();
    int fiq = func_0206d9cc();
    int locked = 0;
    int took = 0;
    if (OS_TryLockCartridge(0x70) == 0) {
        took = 1;
        locked = 1;
    } else if (OS_ReadOwnerOfLockCartridge() & OS_MAINP_LOCKED_FLAG) {
        locked = 1;
    }
    if (locked) {
        int f = 0;
        if (irq) {
            f |= 1;
        }
        if (fiq) {
            f |= 2;
        }
        if (took) {
            f |= 0x20;
        }
        *out = f;
    } else {
        ((void (*)(int))IRQ::SetFIQState)(fiq);
        ((void (*)(int))IRQ::SetIRQState)(irq);
        *out = 0;
    }
    return locked;
}
}
