//cpp
/* ARM9 cache primitives, .text 0x02058bb0..0x02058c84.
 * HAND-ASM PRIMITIVE: the CP15 cache operations require coprocessor writes.
 * Ordinary C helpers following this range have separate compiler inputs.
 */

namespace CP15 {

// @symbol _ZN4CP1514FlushDataCacheEv
asm void FlushDataCache(void)
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

/* Clean and invalidate the whole data cache by set and way (c7,c14,2). Same
 * traversal as FlushDataCache: r1 walks the four ways in the top two bits, r0
 * walks the 32 lines of each set; the outer loop ends when r1 wraps to zero. */
// @symbol _ZN4CP1527FlushAndInvalidateDataCacheEv
asm void FlushAndInvalidateDataCache(void)
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

// @symbol _ZN4CP1519InvalidateDataCacheEjj
asm void InvalidateDataCache(unsigned int addr, unsigned int size)
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

// @symbol _ZN4CP1514FlushDataCacheEjj
asm void FlushDataCache(unsigned int addr, unsigned int size)
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

// @symbol _ZN4CP1527FlushAndInvalidateDataCacheEjj
asm void FlushAndInvalidateDataCache(unsigned int addr, unsigned int size)
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

// @symbol _ZN4CP1516DrainWriteBufferEv
asm void DrainWriteBuffer(void)
{
    mov r0, #0
    mcr p15, 0, r0, c7, c10, 4
    bx lr
}

// @symbol _ZN4CP1526InvalidateInstructionCacheEjj
asm void InvalidateInstructionCache(unsigned int addr, unsigned int size)
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

} // namespace CP15
