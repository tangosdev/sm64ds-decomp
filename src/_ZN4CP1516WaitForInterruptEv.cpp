//cpp
// @symbol _ZN4CP1516WaitForInterruptEv
/* CP15::WaitForInterrupt() at 0x02059d80 -- halt the core until an interrupt arrives (c7,c0,4 is the wait-for-interrupt
 * op). The value written is ignored by the hardware but the register still has
 * to be materialized.
 *
 * HAND-ASM PRIMITIVE: `mrc`/`mcr p15` are coprocessor accesses and no C
 * construct compiles to them, so the asm block is the faithful source rather
 * than a transcription of something lost. The asm-primitive policy is unchanged
 * by this migration; what changes is only that the compiler mangles the symbol
 * instead of the file spelling it by hand. Layout-free (plan phase 1): no
 * `this`, no vtable, no struct, no includers. */
namespace CP15 {

void WaitForInterrupt(void)
{
    unsigned int v = 0;
    asm { mcr p15,0,v,c7,c0,4 }
}

}
