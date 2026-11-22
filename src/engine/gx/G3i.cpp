//cpp
/* engine/gx/G3i.cpp -- the Nitro SDK's gx_g3_util.c (arm9 0x02055998..0x02055dec)
 *
 * The geometry engine's matrix builders, under the SDK's names with C
 * linkage (include/nitro/gx/g3_util.h; ntrtwl/NitroSDK
 * libraries/gx/src/g3_util.c): G3i_LookAt_ builds a camera basis and loads
 * it as the position/vector matrix, G3i_PerspectiveW_ runs the divider to
 * build a perspective projection and loads it as the projection matrix.
 * Both optionally store the matrix to the caller's as well.
 *
 * G3_LoadTexMtxEnv, the TU's first function, selects the texture matrix mode
 * and loads a 4x4 matrix, the first twelve words shifted left by 4.
 *
 * PerspectiveW_ and LookAt_ stream their matrix through a port pointer
 * bound inside the draw branch: the ROM holds that address in a register
 * across the divider and VEC calls, and writing reg_G3_MTX_LOAD_* at each
 * store instead reloads it and grows both functions.
 */
#include "nitro/gx/g3_util.h"
#include "math/Matrix.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/gx/g3imm_cmd.h"
#include "math/CalcLib.h"

#define reg_CP_DIV_NUMER    (*(volatile u64 *)0x04000290)
#define reg_CP_DIV_DENOM    (*(volatile u64 *)0x04000298)
#define reg_G3_MTX_MODE     (*(volatile u32 *)0x04000440)
#define reg_G3_MTX_LOAD_4x4 (*(volatile s32 *)0x04000458)
#define reg_G3_MTX_LOAD_4x3 (*(volatile s32 *)0x0400045c)

/* The SDK's fx_cp.h inlines: start a 64/32 division without waiting. */
static inline void FX_DivAsyncImm(fx32 numer, fx32 denom) {
    reg_CP_DIV_NUMER = (u64)(u32)numer << 32;
    reg_CP_DIV_DENOM = (u32)denom;
}

static inline void FX_InvAsyncImm(fx32 denom) {
    reg_CP_DIV_NUMER = (u64)FX32_ONE << 32;
    reg_CP_DIV_DENOM = (u32)denom;
}

/* The Nitro SDK's G3i_PerspectiveW_: the draw flag is a BOOL. Spelled bool,
   the body tests it differently from the ROM; int reproduces it. The matrix
   is the SDK's MtxFx44, sixteen words. */
void G3i_PerspectiveW_(fx32 fovySin, fx32 fovyCos, fx32 aspect, fx32 n, fx32 f, fx32 scaleW,
                       int draw, Matrix4x3 *mtxp) {
    MtxFx44 *mtx = (MtxFx44 *)mtxp;
    fx32 fovyCot;
    volatile s32 *port;
    fx32 xScale;
    fx64c invNearMinusFar;
    fx32 zScale;
    fx32 nearFar2;
    fx32 wScale;

    fovyCot = FX_Div(fovyCos, fovySin);
    if (scaleW != FX32_ONE)
        fovyCot = fovyCot * scaleW / FX32_ONE;

    FX_DivAsyncImm(fovyCot, aspect);
    if (draw) {
        reg_G3_MTX_MODE = GX_MTXMODE_PROJECTION;
        port = &reg_G3_MTX_LOAD_4x4;
    }
    if (mtx) {
        mtx->m[0][1] = 0;
        mtx->m[0][2] = 0;
        mtx->m[0][3] = 0;
        mtx->m[1][0] = 0;
        mtx->m[1][2] = 0;
        mtx->m[1][3] = 0;
        mtx->m[2][0] = 0;
        mtx->m[2][1] = 0;
        mtx->m[2][3] = -scaleW;
        mtx->m[3][0] = 0;
        mtx->m[3][1] = 0;
        mtx->m[3][3] = 0;
    }
    xScale = FX_GetDivResult();
    FX_InvAsyncImm(n - f);
    if (draw) {
        *port = xScale;
        *port = 0;
        *port = 0;
        *port = 0;
        *port = 0;
        *port = fovyCot;
        *port = 0;
        *port = 0;
        *port = 0;
        *port = 0;
    }
    if (mtx) {
        mtx->m[0][0] = xScale;
        mtx->m[1][1] = fovyCot;
    }
    invNearMinusFar = FX_GetDivResultFx64c();
    if (scaleW != FX32_ONE)
        invNearMinusFar = invNearMinusFar * scaleW / FX32_ONE;
    zScale = (((f + n) * invNearMinusFar + 0x80000000LL) >> 32);
    nearFar2 = FX_MUL(n << 1, f);
    wScale = ((invNearMinusFar * nearFar2 + 0x80000000LL) >> 32);
    if (draw) {
        *port = zScale;
        *port = -scaleW;
        *port = 0;
        *port = 0;
        *port = wScale;
        *port = 0;
    }
    if (mtx) {
        mtx->m[2][2] = zScale;
        mtx->m[3][2] = wScale;
    }
}

void G3i_LookAt_(const VecFx32 *camPos, const VecFx32 *camUp, const VecFx32 *target, int draw,
                 MtxFx43 *mtx) {
    VecFx32 vLook;
    VecFx32 vRight;
    VecFx32 vUp;
    fx32 tx;
    fx32 ty;
    fx32 tz;
    volatile s32 *port;

    /* The view's z axis, from the target back toward the camera (the
     * camera looks down -z). */
    vLook.x = camPos->x - target->x;
    vLook.y = camPos->y - target->y;
    vLook.z = camPos->z - target->z;
    VEC_Normalize(&vLook, &vLook);

    VEC_CrossProduct(camUp, &vLook, &vRight);
    VEC_Normalize(&vRight, &vRight);

    VEC_CrossProduct(&vLook, &vRight, &vUp);

    if (draw) {
        reg_G3_MTX_MODE = GX_MTXMODE_POSITION_VECTOR;
        port = &reg_G3_MTX_LOAD_4x3;
        *port = vRight.x;
        *port = vUp.x;
        *port = vLook.x;
        *port = vRight.y;
        *port = vUp.y;
        *port = vLook.y;
        *port = vRight.z;
        *port = vUp.z;
        *port = vLook.z;
    }

    tx = -VEC_DotProduct(camPos, &vRight);
    ty = -VEC_DotProduct(camPos, &vUp);
    tz = -VEC_DotProduct(camPos, &vLook);

    if (draw) {
        *port = tx;
        *port = ty;
        *port = tz;
    }

    if (mtx == 0) {
        return;
    }

    mtx->m[0][0] = vRight.x;
    mtx->m[0][1] = vUp.x;
    mtx->m[0][2] = vLook.x;
    mtx->m[1][0] = vRight.y;
    mtx->m[1][1] = vUp.y;
    mtx->m[1][2] = vLook.y;
    mtx->m[2][0] = vRight.z;
    mtx->m[2][1] = vUp.z;
    mtx->m[2][2] = vLook.z;
    mtx->m[3][0] = tx;
    mtx->m[3][1] = ty;
    mtx->m[3][2] = tz;
}

extern "C" void G3_LoadTexMtxEnv(int *m) {
    reg_G3_MTX_MODE = GX_MTXMODE_TEXTURE;
    reg_G3_MTX_LOAD_4x4 = m[0] << 4;
    reg_G3_MTX_LOAD_4x4 = m[1] << 4;
    reg_G3_MTX_LOAD_4x4 = m[2] << 4;
    reg_G3_MTX_LOAD_4x4 = m[3] << 4;
    reg_G3_MTX_LOAD_4x4 = m[4] << 4;
    reg_G3_MTX_LOAD_4x4 = m[5] << 4;
    reg_G3_MTX_LOAD_4x4 = m[6] << 4;
    reg_G3_MTX_LOAD_4x4 = m[7] << 4;
    reg_G3_MTX_LOAD_4x4 = m[8] << 4;
    reg_G3_MTX_LOAD_4x4 = m[9] << 4;
    reg_G3_MTX_LOAD_4x4 = m[10] << 4;
    reg_G3_MTX_LOAD_4x4 = m[11] << 4;
    reg_G3_MTX_LOAD_4x4 = m[12];
    reg_G3_MTX_LOAD_4x4 = m[13];
    reg_G3_MTX_LOAD_4x4 = m[14];
    reg_G3_MTX_LOAD_4x4 = m[15];
}
