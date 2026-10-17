//cpp
/* CP15 coprocessor primitives, 0x0206da64..0x0206dab4.
 * Second CP15 run: three unnamed p15 primitives plus the two MPU region-7
 * methods, not contiguous with the 0x02058bb0 body in CP15.cpp. The cstd tail
 * run begins at func_0206dab4, so this run is its own file. */
// leftovers: the three func_0206da6x/94/9c primitives keep address names (no
// recovered names exist). Every body is asm: mrc/mcr p15 is unreachable from
// C, so the asm is the faithful source.
#include "types.h"

extern "C" {
// HAND-ASM PRIMITIVE: CP15 control-register primitive; mrc/mcr p15 are
// unreachable from C, so the asm block is the source.
// @symbol func_0206da64
asm int func_0206da64(void) {
    mov r0, #1
    b lbl
    mov r0, #0
    b lbl
lbl:
    mrc p15, 0, r1, c1, c0, 0
    and r2, r1, #1
    tst r0, r0
    biceq r1, r1, #1
    orrne r1, r1, #1
    mcr p15, 0, r1, c1, c0, 0
    mov r0, r2
    bx lr
}

// HAND-ASM PRIMITIVE: CP15 cache primitive; mrc p15 is unreachable from C, so
// the asm block is the source.
// @symbol func_0206da94
asm void func_0206da94(void) { mrc p15, 0, r0, c7, c10, 1; bx lr }

// HAND-ASM PRIMITIVE: CP15 cache primitive; mrc p15 is unreachable from C, so
// the asm block is the source.
// @symbol func_0206da9c
asm void func_0206da9c(void) { mrc p15, 0, r0, c7, c6, 1; bx lr }
}

#pragma push
#pragma defer_codegen off
// @symbol _ZN4CP1517MPUGetDataRegion7Ev
/* Read back MPU data region 7 (c6,c7).
 * HAND-ASM PRIMITIVE: mrc p15 is unreachable from C, so the asm block is the
 * source. */
namespace CP15 {

unsigned int MPUGetDataRegion7(void)
{
    unsigned int v;
    asm { mrc p15,0,v,c6,c7,0 }
    return v;
}

}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol _ZN4CP1514MPUDataRegion7Ej
/* Write MPU data region 7 (c6,c7).
 * HAND-ASM PRIMITIVE: mcr p15 is unreachable from C, so the asm block is the
 * source. */
namespace CP15 {

void MPUDataRegion7(unsigned int x)
{
    asm { mcr p15,0,x,c6,c7,0 }
}

}
#pragma pop
