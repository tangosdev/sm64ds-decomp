//cpp
/* recovered: project a world point into DS screen pixels through an ov007 camera.
 *
 * The point at `pos` is transformed by the camera's viewMatrix into a local
 * camera-space triple. The camera's 16-bit fovy, reduced by >> 4, indexes the
 * arm9 SINE_TABLE (FX_SinCosTable_) in PAIRS: the even half multiplies the absolute depth
 * out[2] to give the denominator, and FX_Div divides the odd half by it. Everything
 * is 1.12 fixed point, so each product is rounded with + 0x800 and shifted down by 12.
 * The resulting factor scales the x and y components, and the last two lines land them
 * on the screen: x times 0x60 about the 0x80 centre column, y negated about the 0x60
 * centre row.
 *
 * Both callers (func_ov007_020be0dc, func_ov007_020be7a8) pass the manager's
 * cameras[0].
 *
 * ONE SPELLING IS LOAD-BEARING and it is a declaration RANK, not a type (notes 6cc/6k):
 * `z` and its absolute-value fix-up are read FIRST, ahead of the angle, the table base
 * and the two table reads. With `z` read in its natural place next to the table reads
 * the function is still exactly 0xdc bytes with every instruction shape, immediate and
 * branch right, and seven words differ as a pure four-register rotation (the pool base
 * ip vs r0, `j` r2 vs ip, the second byte offset lr vs ip, the 0x800 constant r0 vs r1).
 * Hoisting the `z` pair to the top of the block is the whole match; widening any local
 * to `long` on top of it is byte-inert, and no launder, transposition or pragma reaches
 * it (492-cell pragma sweep and ~6,000 permuter iterations, both flat at 7).
 */
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Ov007Camera.h"
#include "nitro/fx/fx_cp.h"
#include "math/MtxStack.h"

extern "C" {

void func_ov007_020bfd70(Ov007Camera *cam, void *pos, int *outX, int *outY) {
    int out[3];
    MTX_MultVec43((Vector3 *)pos, &cam->viewMatrix, (Vector3 *)out);
    int z = out[2];
    if (z < 0)
        z = -z;
    int ang = cam->fovy >> 4;
    int sinv = FX_SinIdx(ang);
    int cosv = FX_CosIdx(ang);
    int r = FX_MUL(sinv, z);
    int f = FX_Div(cosv, r);
    int a = FX_MUL(f, out[1]);
    int b = FX_MUL(f, out[0]);
    outX[0] = ((b * 0x60) >> 0xc) + 0x80;
    outY[0] = ((-a * 0x60) >> 0xc) + 0x60;
}
}
