//cpp
/* The tilting platforms' shared base -- ov002/daObjGuragura_c.
 * Abstract: slots 0 (InitResources) and 3 (CleanupResources) are pure.
 * daObjKm2_Gura_c (ov045) and daObjFl_Gura_c (ov064) call
 * InitResourcesFrom / CleanupResourcesFrom with their own file tables.
 *
 * The slab tilts mTilt toward mTiltTarget, arms mSettleDelay on a landing,
 * and reloads the rest pose from data_02092768 when that countdown hits 0.
 *
 * What stays, and why:
 * - StoodOnCallback keeps #pragma long_calls -- the ROM veneer is the
 *   pooled `ldr ip,[pc,#8]; bx ip` absolute tail-call (size 0x14); a near
 *   `b` to TiltToward in this same TU is 0xc.
 * - func_020393c4 is a 4-byte store into dBgW+0x1c. Naming belongs
 *   with dBgW in arm9.
 * - Quaternion_SLerp / FromVector3 / Normalize stay untyped (mTilt
 *   is s32[4]).
 * - Rest-pose data_02092768 and scratch matrix data_020a0e68 (arm9).
 *   math/MtxStack.h types MulMat4x3Mat4x3 as const int *, so this file
 *   keeps its own Matrix4x3 * spellings of the matrix helpers.
 * - Toucher position is (Vector3 *)(arg + 0x5c); the callback arg
 *   is not a typed actor here.
 * - Abstract class -- no factory, no g_profile row (S14).
 */

#pragma defer_codegen off

#include "daObjGuragura_c.h"
#include "SharedFilePtr.h"
#include "math/CalcLib.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern Matrix4x3 data_020a0e68;
extern int data_02092768[4];

/* local extern: arm9 matrix, quaternion and dBgW helpers with no shared
 * header that agrees with these spellings. */
extern "C" {
void InvMat4x3(Matrix4x3 *d, Matrix4x3 *s);
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *out);
void Quaternion_FromVector3(int *q, Vector3 *from, Vector3 *to);
void Quaternion_Normalize(int *q);
void Quaternion_SLerp(char *out, char *a, int t, char *b);
void Matrix4x3_FromQuaternion(void *q, Matrix4x3 *m);
void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void MulMat4x3Mat4x3(Matrix4x3 *a, void *b, void *c);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, short a);
void func_020393c4(dBgW_KcMbg *clsn, void *fn);
}

/* dActor_c::mFlags bit 0x08 is written by the framework while the actor is
 * off screen. This int-returning predicate is what makes the ROM
 * materialize 0/1 before testing it. */
static inline int IsOffScreen(dActor_c *actor) {
    return (actor->mFlags & 8) != 0;
}

// @symbol _ZN15daObjGuragura_cD0Ev
// @symbol _ZN15daObjGuragura_cD1Ev
/* ROM ordinals 0 and 1 -- ov002 0x020b5fd8 (D0, 0x58) and 0x020b6030 (D1,
 * 0x44). No source here: both destructor variants come from the ONE
 * inline body in include/daObjGuragura_c.h, which the class's descendants need
 * visible to inline its vptr store.
 *
 * The two calls below are never executed. Under `#pragma defer_codegen off`
 * the compiler emits ordinary functions at parse time in source order, so the
 * delete-expression pulls the deleting variant out of line first and the
 * explicit destructor call pulls the complete-object variant out second --
 * the cartridge's D0-then-D1 order, which no deferred form reaches without a
 * D2 the image does not contain. A delete-expression for D0 because
 * dBgActor_c declares Kill, a key function reachable from this class. */

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void daObjGuragura_c_EmitDeletingDestructor(daObjGuragura_c *p) {
    delete p;
}

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjGuragura_c_EmitDestructor(daObjGuragura_c *p) {
    p->~daObjGuragura_c();
}

/* ROM ordinal 2 -- 0x020b6074, size 0x88.
 * Per-frame tilt: live quaternion to a matrix, offset by position >> 3,
 * apply mAngleY, publish as the model matrix. */
// @symbol _ZN15daObjGuragura_c17UpdateModelMatrixEv
void daObjGuragura_c::UpdateModelMatrix() {
    Matrix4x3 q;
    Vector3 v;
    Matrix4x3_FromQuaternion(mTilt, &q);
    Vec3_Asr(&v, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    MulMat4x3Mat4x3(&q, &data_020a0e68, &data_020a0e68);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    mModel.mat4x3 = data_020a0e68;
}

/* ROM ordinal 3 -- 0x020b60fc, size 0x48.
 * Teardown half both leaves call: disable the mesh if live, then release
 * the leaf's two SharedFilePtrs. */
// @symbol _ZN15daObjGuragura_c20CleanupResourcesFromEP18ResourceDescriptor
s32 daObjGuragura_c::CleanupResourcesFrom(ResourceDescriptor *descriptor) {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    descriptor->model->Release();
    descriptor->collision->Release();
    return 1;
}

/* ROM ordinal 4 -- vtable slot 9, ov002 0x020b6144. Key function. */
// @symbol _ZN15daObjGuragura_c6RenderEv
s32 daObjGuragura_c::Render() {
    mModel.Render(0);
    return 1;
}

/* ROM ordinal 5 -- vtable slot 6, ov002 0x020b616c. */
// @symbol _ZN15daObjGuragura_c8BehaviorEv
s32 daObjGuragura_c::Behavior() {
    if (IsOffScreen(this)) {
        if (mMeshCollider.IsEnabled()) {
            mMeshCollider.Disable();
        }
        return 1;
    }
    if (DecIfAbove0_Byte(&mSettleDelay) == 0) {
        mTiltTarget[0] = data_02092768[0];
        mTiltTarget[1] = data_02092768[1];
        mTiltTarget[2] = data_02092768[2];
        mTiltTarget[3] = data_02092768[3];
    }
    Quaternion_SLerp((char *)mTilt, (char *)mTiltTarget, 0x199, (char *)mTilt);
    UpdateModelMatrix();
    if (IsClsnInRange(0, 0)) {
        UpdateClsnPosAndRot();
    }
    mBumped = 0;
    return 1;
}

/* ROM ordinal 6 -- 0x020b6244, size 0x88.
 * Shared resource setup both leaves call. Slot 0 is dExtSimpleModel_c::LoadFile, slot 1
 * is dBgW_Kc::LoadFile, slot 2 is CLPS into SetFile; then install
 * StoodOnCallback as the collider callback. */
// @symbol _ZN15daObjGuragura_c17InitResourcesFromEP18ResourceDescriptor
s32 daObjGuragura_c::InitResourcesFrom(ResourceDescriptor *descriptor) {
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(*descriptor->model), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mMeshCollider.SetFile((KCL_File *)dBgW_Kc::LoadFile(*descriptor->collision), mClsnMat, 0x1000,
                          mAngleY, *descriptor->clps);
    func_020393c4(&mMeshCollider, (void *)&StoodOnCallback);
    return 1;
}

/* ROM ordinal 7 -- 0x020b62cc, size 0xa8.
 * Landing on the slab: flag mBumped, invert mClsnMat through the arm9
 * scratch matrix, double the toucher's vertical component in slab space,
 * and make mTiltTarget the rotation that carries +Y onto it. Arms
 * mSettleDelay with 10 frames. */
// @symbol _ZN15daObjGuragura_c10TiltTowardEPh
void daObjGuragura_c::TiltToward(unsigned char *arg) {
    Vector3 v;
    Vector3 axis;
    mBumped = 1;
    data_020a0e68 = mClsnMat;
    InvMat4x3(&data_020a0e68, &data_020a0e68);
    MulVec3Mat4x3((Vector3 *)(arg + 0x5c), &data_020a0e68, &v);
    v.y = v.y << 1;
    axis.x = 0;
    axis.z = 0;
    axis.y = 0x1000;
    Quaternion_FromVector3(mTiltTarget, &axis, &v);
    Quaternion_Normalize(mTiltTarget);
    mSettleDelay = 0xa;
}

/* ROM ordinal 8 -- 0x020b6374, size 0x14.
 * dBgW callback InitResourcesFrom installs: drops the collider and forwards
 * the other two into TiltToward. long_calls is the ROM's pooled absolute
 * tail-call. */
// @symbol _ZN15daObjGuragura_c15StoodOnCallbackEPvPS_Ph
#pragma long_calls on
void daObjGuragura_c::StoodOnCallback(void *collider, daObjGuragura_c *self, unsigned char *arg) {
    self->TiltToward(arg);
}
#pragma long_calls off
