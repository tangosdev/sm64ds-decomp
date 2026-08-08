//cpp
// @symbol _ZN4CP1527FlushAndInvalidateDataCacheEv
/* CP15::FlushAndInvalidateDataCache() at 0x02058bdc -- clean and invalidate the whole data cache by set and way (c7,c14,2). Same
 * traversal as the range-free FlushDataCache.
 *
 * HAND-ASM PRIMITIVE: `mrc`/`mcr p15` are coprocessor accesses and no C
 * construct compiles to them, so the asm block is the faithful source rather
 * than a transcription of something lost. The asm-primitive policy is unchanged
 * by this migration; what changes is only that the compiler mangles the symbol
 * instead of the file spelling it by hand. Layout-free (plan phase 1): no
 * `this`, no vtable, no struct, no includers. */
namespace CP15 {

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

}
