/* OS_SpinWait (the Nitro SDK's os_system.c): burns about `cycles` CPU cycles,
 * four per loop pass. It is the last function of that SDK file (pret's
 * OS_system.c lines up with 0x02059d08..0x02059d98 8 for 8); the rest is
 * src/IRQ3.cpp.
 *
 * HAND-ASM PRIMITIVE: bhs reads the CARRY flag, which is a property of the
 * value before the subtraction. C has no expression over the post-subtraction
 * result that denotes carry, and mwccarm fuses a subtract with its following
 * test only when the test is against zero (Z/N), so the HS predicate is
 * unreachable at the exact 12-byte shape. Established across 20 mwccarm builds
 * x 7 optimization levels x 8 pragmas.
 */
#include "nitro/os/system.h"

#ifndef SM64DS_PLATFORM_PC
asm void OS_SpinWait(u32 cycles)
{
L0:
    subs r0, r0, #4;
    bhs L0;
    bx lr
}
#else
void OS_SpinWait(u32 cycles)
{
    /* subs/bhs: four cycles per pass, stop once fewer than four remain. */
    while (cycles >= 4)
        cycles -= 4;
}
#endif
