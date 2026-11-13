//cpp
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Ov007Camera.h"
#include "nitro/fx/fx_cp.h"
#include "math/MtxStack.h"

extern "C" {



/* Unprojects the screen point (a, b), in 20.12 pixels, at depth c through
 * the camera's fovy and inverse view matrix. */
void func_ov007_020bfe4c(Ov007Camera *cam, int a, int b, int c, Vector3 *out) {
    Vector3 v;
    Matrix4x3 inv;
    int ang, sinv, cosv, ac, f1;
    v.x = a - 0x80000;
    v.y = -(b - 0x60000);
    v.z = c;
    ang = (int)cam->fovy >> 4;
    cosv = FX_CosIdx(ang);
    sinv = FX_SinIdx(ang);
    ac = c < 0 ? -c : c;
    f1 = FX_Div(FX_MUL(sinv, ac), cosv);
    v.x = FX_DivS32(FX_MUL(v.x, f1), 0x60);
    v.y = FX_DivS32(FX_MUL(v.y, f1), 0x60);
    MTX_Inverse43(&cam->viewMatrix, &inv);
    MTX_MultVec43(&v, &inv, out);
}
}
