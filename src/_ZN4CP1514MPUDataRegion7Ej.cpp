//cpp
// @symbol _ZN4CP1514MPUDataRegion7Ej
/* CP15::MPUDataRegion7(u32) at 0x0206daac -- write MPU data region 7 (c6,c7).
 *
 * HAND-ASM PRIMITIVE: `mrc`/`mcr p15` are coprocessor accesses and no C
 * construct compiles to them, so the asm block is the faithful source rather
 * than a transcription of something lost. The asm-primitive policy is unchanged
 * by this migration; what changes is only that the compiler mangles the symbol
 * instead of the file spelling it by hand. Layout-free (plan phase 1): no
 * `this`, no vtable, no struct, no includers. */
namespace CP15 {

void MPUDataRegion7(unsigned int x)
{
    asm { mcr p15,0,x,c6,c7,0 }
}

}
