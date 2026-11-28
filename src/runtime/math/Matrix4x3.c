/* Game affine-matrix construction and in-place application helpers. These
 * are thin wrappers over the NitroSDK 4x3 matrix routines; mwccarm emits the
 * compiled C bodies in reverse source order. */
#include "types.h"
#include "common.h"
#include "nitro/fx/fx_trig.h"
#include "math/Matrix.h"
#include "math/MtxStack.h"

/* Matrix4x3_ApplyInPlaceToTranslation at 0x0203c184
 * Builds a translation matrix on the stack, then multiplies mF by it in-place.
 */

void Matrix4x3_ApplyInPlaceToTranslation(union Matrix4x3 *mF, fx32 x, fx32 y, fx32 z) {
    union Matrix4x3 trans;
    Matrix4x3_FromTranslation(&trans, x, y, z);
    MTX_Concat43(&trans, mF, mF);
}

/* func_0203c178 @ 0x203c178 (arm9) -- tail-call veneer to MTX_Scale43_ (0x20527e8).
 * ldr ip, [pc]; bx ip; .word 0x20527e9 (Thumb bit set on the BX target; the
 * real symbol, config/arm9/symbols.txt, is MTX_Scale43_ thumb size 0x16)
 */

extern void MTX_Scale43_(Matrix4x3 *m, int sx, int sy, int sz);

/* (m, sx, sy, sz) pass straight through in r0-r3 to the Thumb body, which
   builds the 4x3 scale matrix (NitroSDK's MTX_Scale43_ shape). */
void func_0203c178(void *m, int sx, int sy, int sz) {
    MTX_Scale43_(m, sx, sy, sz);
}

/* Matrix4x3_ApplyInPlaceToScale at 0x0203c148
 * Builds a scale matrix on the stack (Thumb), then multiplies mF by it in-place.
 */

extern void MTX_Scale43_(union Matrix4x3 *mF, fx32 x, fx32 y, fx32 z)
    __attribute__((long_call, target("thumb")));

void Matrix4x3_ApplyInPlaceToScale(union Matrix4x3 *mF, fx32 x, fx32 y, fx32 z) {
    union Matrix4x3 scale;
    MTX_Scale43_(&scale, x, y, z);
    MTX_Concat43(&scale, mF, mF);
}

void Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3 *m, int x, int y, int z) {
    Matrix4x3 rot;
    if (z != 0) {
        Matrix4x3_FromRotationZ(&rot, z);
        MTX_Concat43(&rot, m, m);
    }
    if (y != 0) {
        Matrix4x3_FromRotationY(&rot, y);
        MTX_Concat43(&rot, m, m);
    }
    if (x != 0) {
        Matrix4x3_FromRotationX(&rot, x);
        MTX_Concat43(&rot, m, m);
    }
}

void Matrix4x3_ApplyInPlaceToRotationZXYExt(Matrix4x3 *m, int x, int y, int z) {
    Matrix4x3 rot;
    if (y != 0) {
        Matrix4x3_FromRotationY(&rot, y);
        MTX_Concat43(&rot, m, m);
    }
    if (x != 0) {
        Matrix4x3_FromRotationX(&rot, x);
        MTX_Concat43(&rot, m, m);
    }
    if (z != 0) {
        Matrix4x3_FromRotationZ(&rot, z);
        MTX_Concat43(&rot, m, m);
    }
}

/* Matrix4x3_ApplyInPlaceToRotationX at 0x0203bff0
 * Builds a rotation-X matrix on the stack, then multiplies mF by it in-place.
 */

void Matrix4x3_ApplyInPlaceToRotationX(union Matrix4x3 *mF, s16 angX) {
    union Matrix4x3 rot;
    Matrix4x3_FromRotationX(&rot, angX);
    MTX_Concat43(&rot, mF, mF);
}

/* Matrix4x3_ApplyInPlaceToRotationY at 0x0203bfc0
 * Builds a rotation-Y matrix on the stack, then multiplies mF by it in-place.
 */

void Matrix4x3_ApplyInPlaceToRotationY(union Matrix4x3 *mF, s16 angY) {
    union Matrix4x3 rot;
    Matrix4x3_FromRotationY(&rot, angY);
    MTX_Concat43(&rot, mF, mF);
}

/* Matrix4x3_ApplyInPlaceToRotationZ at 0x0203bf90
 * Builds a rotation-Z matrix on the stack, then multiplies mF by it in-place.
 */

void Matrix4x3_ApplyInPlaceToRotationZ(union Matrix4x3 *mF, s16 angZ) {
    union Matrix4x3 rot;
    Matrix4x3_FromRotationZ(&rot, angZ);
    MTX_Concat43(&rot, mF, mF);
}

void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z) {
    m->r.m[0][0] = 0x1000;
    m->r.m[0][1] = 0;
    m->r.m[0][2] = 0;
    m->r.m[1][0] = 0;
    m->r.m[1][1] = 0x1000;
    m->r.m[1][2] = 0;
    m->r.m[2][0] = 0;
    m->r.m[2][1] = 0;
    m->r.m[2][2] = 0x1000;
    m->t.x = x;
    m->t.y = y;
    m->t.z = z;
}

void Matrix4x3_FromRotationXYZExt(Matrix4x3 *m, int x, int y, int z) {
    Matrix4x3 rot;
    if (z != 0)
        Matrix4x3_FromRotationZ(m, z);
    else
        *m = IDENTITY_MATRIX4X3;
    if (y != 0) {
        Matrix4x3_FromRotationY(&rot, y);
        MTX_Concat43(&rot, m, m);
    }
    if (x != 0) {
        Matrix4x3_FromRotationX(&rot, x);
        MTX_Concat43(&rot, m, m);
    }
}

void Matrix4x3_FromRotationZXYExt(Matrix4x3 *m, int x, int y, int z) {
    Matrix4x3 rot;
    if (y != 0)
        Matrix4x3_FromRotationY(m, y);
    else
        *m = IDENTITY_MATRIX4X3;
    if (x != 0) {
        Matrix4x3_FromRotationX(&rot, x);
        MTX_Concat43(&rot, m, m);
    }
    if (z != 0) {
        Matrix4x3_FromRotationZ(&rot, z);
        MTX_Concat43(&rot, m, m);
    }
}

void Matrix4x3_FromRotationX(Matrix4x3 *m, int angle) {
    int a = (unsigned short)angle >> 4;
    MTX_RotX43_(m, FX_SinIdx(a), FX_CosIdx(a));
}

void Matrix4x3_FromRotationY(Matrix4x3 *m, int angle) {
    int a = (unsigned short)angle >> 4;
    MTX_RotY43_(m, FX_SinIdx(a), FX_CosIdx(a));
}

void Matrix4x3_FromRotationZ(Matrix4x3 *m, int angle) {
    int a = (unsigned short)angle >> 4;
    MTX_RotZ43_(m, FX_SinIdx(a), FX_CosIdx(a));
}
