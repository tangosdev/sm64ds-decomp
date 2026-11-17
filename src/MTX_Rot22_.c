/* MTX_Rot22_ @ 0x020524e4 (arm9, Thumb): stores (cos, sin, -sin, cos) into
 * the matrix's m00, m01, m10, m11. Identified as the Nitro SDK's FX_mtx22.c
 * MTX_Rot22_ from pret's FX_mtx22.c, whose body is the same five instructions.
 * The SDK's MtxFx22 is this tree's Matrix2x2 (include/nitro/gx/g2.h).
 * str r2,[r0]; str r1,[r0,#4]; negs r1,r1; str r1,[r0,#8]; str r2,[r0,#0xc]; bx lr
 */
#pragma thumb on
#include "Matrix2x2.h"

void MTX_Rot22_(struct Matrix2x2 *mtx, fx32 sinVal, fx32 cosVal) {
    mtx->m00 = cosVal;
    mtx->m01 = sinVal;
    mtx->m10 = -sinVal;
    mtx->m11 = cosVal;
}
