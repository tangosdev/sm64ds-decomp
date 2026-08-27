//cpp
// @symbol _ZN4CP1510DisableMPUEv
/* CP15::DisableMPU() at 0x020593dc -- clear bit 0 of the control register to turn the MPU off.
 *
 * Note the asymmetry with EnableMPU: this one clears the bit with an asm `bic`
 * rather than C's `v &= ~1`, and that spelling is load-bearing for the match.
 *
 * HAND-ASM PRIMITIVE: `mrc`/`mcr p15` are coprocessor accesses and no C
 * construct compiles to them, so the asm block is the faithful source rather
 * than a transcription of something lost. The asm-primitive policy is unchanged
 * by this migration; what changes is only that the compiler mangles the symbol
 * instead of the file spelling it by hand. Layout-free (plan phase 1): no
 * `this`, no vtable, no struct, no includers. */
namespace CP15 {

unsigned int DisableMPU(void)
{
    unsigned int v;
    asm { mrc p15,0,v,c1,c0,0 }
    asm { bic v,v,#1 }
    asm { mcr p15,0,v,c1,c0,0 }
    return v;
}

}
