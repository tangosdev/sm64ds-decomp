//cpp
/* ARM9 cache primitives (SDK os_cache.c), .text 0x02058bb0..0x02058c84.
 * HAND-ASM PRIMITIVE: the CP15 cache operations require coprocessor writes.
 * Ordinary C helpers following this range have separate compiler inputs.
 *
 * The names are pret/pokediamond's OS_cache.c ones (declared in
 * nitro/os/cache.h), matched by coprocessor operation and loop stride: Store
 * is clean (c7,c10), Flush is clean and invalidate (c7,c14), Invalidate drops
 * the lines (c7,c6; c7,c5 for the instruction cache). This SDK's DC_FlushAll
 * and DC_FlushRange do not drain the write buffer inside the loop; pret's
 * later revision adds a c7,c10,4 before each line.
 */

#include "nitro/os/cache.h"

#ifndef SM64DS_PLATFORM_PC
asm void DC_StoreAll(void)
{
    mov r1, #0
outer:
    mov r0, #0
seg:
    orr r2, r1, r0
    mcr p15, 0, r2, c7, c10, 2
    add r0, r0, #0x20
    cmp r0, #0x400
    blt seg
    add r1, r1, #0x40000000
    cmp r1, #0
    bne outer
    bx lr
}
#endif

/* Clean and invalidate the whole data cache by set and way (c7,c14,2). Same
 * traversal as DC_StoreAll: r1 walks the four ways in the top two bits, r0
 * walks the 32 lines of each set; the outer loop ends when r1 wraps to zero. */
#ifndef SM64DS_PLATFORM_PC
asm void DC_FlushAll(void)
{
    mov r1, #0
outer:
    mov r0, #0
seg:
    orr r2, r1, r0
    mcr p15, 0, r2, c7, c14, 2
    add r0, r0, #0x20
    cmp r0, #0x400
    blt seg
    add r1, r1, #0x40000000
    cmp r1, #0
    bne outer
    bx lr
}
#endif

#ifndef SM64DS_PLATFORM_PC
asm void DC_InvalidateRange(void *addr, u32 size)
{
    add r1, r1, r0
    bic r0, r0, #0x1f
loop:
    mcr p15, 0, r0, c7, c6, 1
    add r0, r0, #0x20
    cmp r0, r1
    blt loop
    bx lr
}
#endif

#ifndef SM64DS_PLATFORM_PC
asm void DC_StoreRange(void *addr, u32 size)
{
    add r1, r1, r0
    bic r0, r0, #0x1f
loop:
    mcr p15, 0, r0, c7, c10, 1
    add r0, r0, #0x20
    cmp r0, r1
    blt loop
    bx lr
}
#endif

#ifndef SM64DS_PLATFORM_PC
asm void DC_FlushRange(const void *addr, u32 size)
{
    add r1, r1, r0
    bic r0, r0, #0x1f
loop:
    mcr p15, 0, r0, c7, c14, 1
    add r0, r0, #0x20
    cmp r0, r1
    blt loop
    bx lr
}
#endif

#ifndef SM64DS_PLATFORM_PC
asm void DC_WaitWriteBufferEmpty(void)
{
    mov r0, #0
    mcr p15, 0, r0, c7, c10, 4
    bx lr
}
#endif

#ifndef SM64DS_PLATFORM_PC
asm void IC_InvalidateRange(void *addr, u32 size)
{
    add r1, r1, r0
    bic r0, r0, #0x1f
loop:
    mcr p15, 0, r0, c7, c5, 1
    add r0, r0, #0x20
    cmp r0, r1
    blt loop
    bx lr
}
#endif
