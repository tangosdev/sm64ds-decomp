//cpp
#include "nitro/fx/fx.h"
#include "cstd.h"
#include "Ov007Camera.h"

extern "C" {

extern void InvMat4x3(void *dst, void *out);
extern void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *res);
extern short data_02082214[];

/* Unprojects the screen point (a, b), in 20.12 pixels, at depth c through
 * the camera's fovy and inverse view matrix. */
void func_ov007_020bfe4c(Ov007Camera *cam, int a, int b, int c, Vector3 *out) {
    Vector3 v;
    Matrix4x3 inv;
    int ang, i, cosv, sinv, ac, f1;
    v.x = a - 0x80000;
    v.y = -(b - 0x60000);
    v.z = c;
    ang = (int)cam->fovy >> 4;
    i = ang * 2;
    sinv = data_02082214[i + 1];
    cosv = data_02082214[i];
    ac = c < 0 ? -c : c;
    f1 = cstd::fdiv(FX_MUL(cosv, ac), sinv);
    v.x = cstd::div(FX_MUL(v.x, f1), 0x60);
    v.y = cstd::div(FX_MUL(v.y, f1), 0x60);
    InvMat4x3(&cam->viewMatrix, &inv);
    MulVec3Mat4x3(&v, &inv, out);
}
}
