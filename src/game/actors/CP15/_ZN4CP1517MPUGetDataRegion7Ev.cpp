//cpp
// @symbol _ZN4CP1517MPUGetDataRegion7Ev
/* CP15::MPUGetDataRegion7() at 0x0206daa4 -- read back MPU data region 7.
 *
 * HAND-ASM PRIMITIVE: `mrc`/`mcr p15` are coprocessor accesses and no C
 * construct compiles to them, so the asm block is the faithful source rather
 * than a transcription of something lost. The asm-primitive policy is unchanged
 * by this migration; what changes is only that the compiler mangles the symbol
 * instead of the file spelling it by hand. Layout-free (plan phase 1): no
 * `this`, no vtable, no struct, no includers. */
namespace CP15 {

unsigned int MPUGetDataRegion7(void)
{
    unsigned int v;
    asm { mrc p15,0,v,c6,c7,0 }
    return v;
}

}
