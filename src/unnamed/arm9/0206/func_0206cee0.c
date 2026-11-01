#include "types.h"
#include "nitro/os/spinLock.h"
extern int func_0206da18(void);
extern int func_0206d9cc(void);
/* This file stays C. IRQ::SetFIQState(bool) measured func_0206d868 0x28 -> 0x40. */
extern void _ZN3IRQ11SetFIQStateEb(int x); /* 0x0206d9dc, inside func_0206d9cc */
extern void _ZN3IRQ11SetIRQStateEb(int x); /* 0x0206da28, inside func_0206da18 */
/* One try at the cartridge lock, with IRQ and FIQ disabled first (the
 * func_0206da18 / func_0206d9cc entries return the previous states). Returns
 * nonzero if the lock was taken with lock id 0x70, or if the main processor
 * already owns it. On success *out gets the flags func_0206ce90 restores
 * from: 1 IRQ was on, 2 FIQ was on, 0x20 this call took the lock. On failure
 * the interrupt states are put back and *out is 0. func_0206cec0 spins on it.
 * The new_var temporaries and the `if (0, new_var2)` test look like permuter
 * output; no cleaner spelling has been tried against the bytes yet. No SDK or
 * MSL function with this body has been found. */
int func_0206cee0(int *out) {
    int v8 = func_0206da18();
    int v7 = func_0206d9cc();
    int new_var;
    int v6 = 0;
    int new_var2;
    int v5 = 0;
    if (OS_TryLockCartridge(0x70) == 0) {
        v5 = 1;
        new_var = 1;
        v6 = new_var;
    } else if (OS_ReadOwnerOfLockCartridge() & OS_MAINP_LOCKED_FLAG) {
        v6 = 1;
    }
    new_var2 = v8;
    if (0 != v6) {
        int f = 0;
        if (0, new_var2) {
            f |= 1;
        }
        if (v7) {
            f |= 2;
        }
        if (v5) {
            f |= 0x20;
        }
        *out = f;
    } else {
        _ZN3IRQ11SetFIQStateEb(v7);
        _ZN3IRQ11SetIRQStateEb(v8);
        *out = 0;
    }
    return v6;
}
