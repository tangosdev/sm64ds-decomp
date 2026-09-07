//cpp
// @symbol _ZN4CP1514FlushDataCacheEjj
/* CP15::FlushDataCache(u32, u32) at 0x02058c24 -- clean the data cache over [addr, addr+size) one 32-byte line at a time
 * (c7,c10,1 = clean by MVA). The start is rounded down to a line boundary; the
 * end is not rounded up, so a range ending mid-line leaves that line alone.
 *
 * HAND-ASM PRIMITIVE: `mrc`/`mcr p15` are coprocessor accesses and no C
 * construct compiles to them, so the asm block is the faithful source rather
 * than a transcription of something lost. The asm-primitive policy is unchanged
 * by this migration; what changes is only that the compiler mangles the symbol
 * instead of the file spelling it by hand. Layout-free (plan phase 1): no
 * `this`, no vtable, no struct, no includers. */
namespace CP15 {

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

}
