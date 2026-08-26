//cpp
// @symbol _ZN4Toad15UpdateModelPoseEv
#include "Toad.h"

/* An array-only wrapper keeps C++ aggregate copies in the same ldm/stm form
 * the original C-shaped match used. */
struct MatrixWords {
    s32 words[12];
};

extern "C" {
extern void Matrix4x3_FromRotationY(void *, int);
extern void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *, s16);
extern void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *, s16);
extern u32 _ZN8dActor_c10FindWithIDEj(u32);
extern void MulMat4x3Mat4x3(void *, void *, void *);
extern void Matrix4x3_ApplyInPlaceToTranslation(
    Matrix4x3 *, int, int, int);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(
    void *, int, int, int);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *, void *, void *, int, int, u32);

extern Matrix4x3 data_020a0e68;
extern Matrix4x3 IDENTITY_MATRIX4X3;
}

void Toad::UpdateModelPose()
{
    char *c = (char *)this;
    Matrix4x3 *dst;
    void *actor;
    u32 id;
    int x, y, z;

    Matrix4x3_FromRotationY(c + 0x124, *(s16 *)(c + 0x8e));
    *(s32 *)(c + 0x148) = *(s32 *)(c + 0x5c) >> 3;
    *(s32 *)(c + 0x14c) = (*(s32 *)(c + 0x60) + 0x4000) >> 3;
    *(s32 *)(c + 0x150) = *(s32 *)(c + 0x64) >> 3;

    *(MatrixWords *)&data_020a0e68 =
        *(MatrixWords *)(*(char **)(c + 0x11c) + 0xf0);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mHeadYaw);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mHeadPitch);
    *(MatrixWords *)(*(char **)(c + 0x11c) + 0xf0) =
        *(MatrixWords *)&data_020a0e68;

    id = *(u32 *)(c + 0x1f4);
    if (id != 0) {
        actor = (void *)_ZN8dActor_c10FindWithIDEj(id);
        if (actor != 0) {
            *(s32 *)&x = 0;
            *(s32 *)&y = 0;
            *(s32 *)&z = 0;
            dst = (Matrix4x3 *)(*(char **)(c + 0x11c) + 0xf0);
            MulMat4x3Mat4x3(dst, c + 0x124, c + 0x194);

            *(MatrixWords *)&data_020a0e68 = *(MatrixWords *)(c + 0x194);
            Matrix4x3_ApplyInPlaceToTranslation(
                &data_020a0e68, 0, 0x4000, -0xa00);
            Matrix4x3_ApplyInPlaceToRotationXYZExt(
                &data_020a0e68, -0x1000, 0, 0);
            *(MatrixWords *)(c + 0x194) = *(MatrixWords *)&data_020a0e68;

            x = ((s32 *)&data_020a0e68)[9];
            y = ((s32 *)&data_020a0e68)[10];
            z = ((s32 *)&data_020a0e68)[11];
            y <<= 3;
            x <<= 3;
            z <<= 3;
            *(s32 *)((char *)actor + 0x5c) = x;
            *(s32 *)((char *)actor + 0x60) = y;
            *(s32 *)((char *)actor + 0x64) = z;
            *(void **)((char *)actor + 0xc8) = c + 0x194;
        }
    }

    *(MatrixWords *)(c + 0x1c4) = *(MatrixWords *)&IDENTITY_MATRIX4X3;
    *(s32 *)(c + 0x1e8) = *(s32 *)(c + 0x5c) >> 3;
    *(s32 *)(c + 0x1ec) = (*(s32 *)(c + 0x60) - 0x8000) >> 3;
    *(s32 *)(c + 0x1f0) = *(s32 *)(c + 0x64) >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x16c, c + 0x1c4, 0x50000, 0x64000, 0xf);
}
