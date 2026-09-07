//cpp
#include "dBgW_KcMbg.h"

extern "C" void InvMat4x3(Matrix4x3* dst, Matrix4x3* src);
extern "C" void Matrix4x3_ApplyInPlaceToScale(Matrix4x3* m, s32 x, s32 y, s32 z);
extern "C" void func_02039e18(void* m, void* v, void* c);
extern "C" int LenVec3(Vector3* v);
extern "C" void func_01ffb0a4(void* self);
extern "C" void NormalizeVec3(Vector3* src, Vector3* dst);
extern "C" int func_01ffb07c(void* self, void* v);
extern "C" void func_020396d0(int* p, int v);
extern "C" void func_01ffb0b0(void* self);
extern "C" void func_01ffb0bc(void* self);

extern Matrix4x3 data_020a0e68;

/* Matrix4x3 and Vector3 carry the shared, ROM-proven Vector3 destructor.
 * Transform predates those cleanup calls and copies both as flat words, so
 * keep that period codegen local without obscuring the collider's real type. */
struct RawMatrix4x3 {
    s32 m[12];
};

struct RawVector3 {
    s32 x, y, z;
};

void dBgW_KcMbg::Transform(const Matrix4x3 &mtx, s16 angle)
{
    *(RawMatrix4x3 *)&invMat = *(RawMatrix4x3 *)&mat;
    InvMat4x3(&invMat, &invMat);

    int zero = 0;
    *(RawMatrix4x3 *)&mat = *(const RawMatrix4x3 *)&mtx;

    s16 oldAngle = angY;
    angVelY = angle - oldAngle;
    angY = angle;

    velocity.x = mtx.t.x - pos.x;
    velocity.y = mtx.t.y - pos.y;
    velocity.z = mtx.t.z - pos.z;
    pos.x = mtx.t.x;
    pos.y = mtx.t.y;
    pos.z = mtx.t.z;

    RawMatrix4x3 rotation = *(RawMatrix4x3 *)&mat;
    rotation.m[9] = zero;
    rotation.m[10] = zero;
    rotation.m[11] = zero;
    *(RawMatrix4x3 *)&invRotMat = rotation;
    InvMat4x3(&invRotMat, &invRotMat);

    *(RawMatrix4x3 *)&scaledMat = *(RawMatrix4x3 *)&newScaledMat;
    *(RawMatrix4x3 *)&data_020a0e68 = *(const RawMatrix4x3 *)&mtx;
    s32 currentScale = scale;
    Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, currentScale, currentScale, currentScale);
    *(RawMatrix4x3 *)&newScaledMat = *(RawMatrix4x3 *)&data_020a0e68;
    *(RawMatrix4x3 *)&prevInvScaledMat = *(RawMatrix4x3 *)&invScaledMat;
    *(RawMatrix4x3 *)&invScaledMat = *(RawMatrix4x3 *)&newScaledMat;
    InvMat4x3(&invScaledMat, &invScaledMat);

    RawVector3 up;
    up.x = 0; up.y = 0x1000; up.z = 0;
    func_02039e18(this, &up, (char *)this + 0x28);

    /* The ROM treats the first byte at 0x130 as the enable flag. Keep the
     * shared header's layout-neutral u32 spelling until its 800+ consumers can
     * be migrated as a dedicated header change. */
    if (*(u8 *)&unk_130 == 0) return;

    RawVector3 v;
    GetVelocity(*(Vector3 *)&v);
    v.y = 0;
    if (LenVec3((Vector3 *)&v) != 0) {
        func_01ffb0a4(this);
        NormalizeVec3((Vector3 *)&v, (Vector3 *)&v);
        RawVector3 v2;
        func_02039e18(this, &v, &v2);
        func_01ffb07c(this, &v2);
        func_020396d0((int*)this, 0xe66);
        func_01ffb0b0(this);
    } else {
        func_01ffb0bc(this);
    }
}
