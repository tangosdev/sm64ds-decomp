/* MI_SwapWord (the Nitro SDK's mi_swap.c, the whole of that file).
 * Atomically stores setData at destp and returns the word that was there.
 * The lock words in src/engine/os/OS_SpinLock.cpp are taken with it.
 *
 * HAND-ASM PRIMITIVE: `swp` has no C spelling under mwccarm, and the SDK
 * writes this function as asm.
 */
#include "nitro/mi/swap.h"

asm u32 MI_SwapWord(u32 setData, volatile u32 *destp)
{
    swp r0, r0, [r1]
    bx lr
}
