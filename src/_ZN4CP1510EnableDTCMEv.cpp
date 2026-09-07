//cpp
// @symbol _ZN4CP1510EnableDTCMEv
/* CP15::EnableDTCM() at 0x020593a8 -- set bit 16 of the control register to enable the data tightly-coupled
 * memory.
 *
 * HAND-ASM PRIMITIVE: `mrc`/`mcr p15` are coprocessor accesses and no C
 * construct compiles to them, so the asm block is the faithful source rather
 * than a transcription of something lost. The asm-primitive policy is unchanged
 * by this migration; what changes is only that the compiler mangles the symbol
 * instead of the file spelling it by hand. Layout-free (plan phase 1): no
 * `this`, no vtable, no struct, no includers. */
namespace CP15 {

unsigned int EnableDTCM(void)
{
    unsigned int v;
    asm { mrc p15,0,v,c1,c0,0 }
    v |= 0x10000;
    asm { mcr p15,0,v,c1,c0,0 }
    return v;
}

}
