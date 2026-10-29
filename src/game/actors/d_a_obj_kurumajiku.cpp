//cpp
/* Rickshaw axle shared by the daKpa_c in the Dark World and daKpa_c in the Sky
 * actors. It turns the model and carries up to four mounted carts.
 *
 * This TU owns ov002 .text 0x020b69e4..0x020b6d28: the two destructors (D0
 * 0x020b69e4, D1 0x020b6a3c) and the five functions below, in ROM order under
 * `#pragma defer_codegen off`, ending with the shared setup helper
 * InitResourcesFrom; the discarded class metadata is recorded in the manifest.
 *
 * Note: data_ov002_0210ddd0 (the four rider mount offsets, defined below)
 * keeps its linker name.
 */

#pragma defer_codegen off

#include "daObjKurumajiku_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "math/MtxStack.h"

typedef char AxleResources_size_must_be_0x0c[sizeof(daObjKurumajiku_c::Resources) == 0x0c ? 1 : -1];

/* The four rider mount offsets, one per cart: +-0x190000 on y and on x,
   each 0x12c000 along z. */
Vector3 data_ov002_0210ddd0[4] = {
    Vector3(0, 0x190000, 0x12c000),
    Vector3(0, -0x190000, 0x12c000),
    Vector3(0x190000, 0, 0x12c000),
    Vector3(-0x190000, 0, 0x12c000),
};

/* local extern: no shared header declares these. */
extern "C" {
void Matrix4x3_FromRotationZXYExt(void *dst, int x, int y, int z);
void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void MulVec3Mat4x3(const Vector3 *v, const void *m, Vector3 *dst);
void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);
}

// @symbol _ZN17daObjKurumajiku_cD0Ev
// @symbol _ZN17daObjKurumajiku_cD1Ev
/* ROM ordinals 0 and 1 -- ov002 0x020b69e4 (D0, 0x58) and 0x020b6a3c (D1,
 * 0x44). No source here: both destructor variants come from the ONE
 * inline body in include/daObjKurumajiku_c.h, which the class's descendants need
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
void daObjKurumajiku_c_EmitDeletingDestructor(daObjKurumajiku_c *p) {
    delete p;
}

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjKurumajiku_c_EmitDestructor(daObjKurumajiku_c *p) {
    p->~daObjKurumajiku_c();
}

// @symbol _ZN17daObjKurumajiku_c20UpdateModelTransformEv
/* Build the model transform. dExtSimpleModel_c translation uses 1/8 of the
   world position. The return value is the z word, which no caller reads. */
int daObjKurumajiku_c::UpdateModelTransform() {
    Matrix4x3_FromRotationZXYExt(&mModel.mat4x3, 0, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    int z = mPosZ >> 3;
    mModel.mat4x3.m[11] = z;
    return z;
}

// @symbol _ZN17daObjKurumajiku_c20CleanupResourcesFromEPNS_9ResourcesE
/* Disable the moving collision before releasing the model and collision files. */
int daObjKurumajiku_c::CleanupResourcesFrom(Resources *resources) {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    resources->model->Release();
    resources->collision->Release();
    return 1;
}

// @symbol _ZN17daObjKurumajiku_c6RenderEv
s32 daObjKurumajiku_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN17daObjKurumajiku_c8BehaviorEv
/* The angle advances even while bit 3 suppresses rider and collision updates. */
s32 daObjKurumajiku_c::Behavior() {
    int i;
    Vector3 mid;
    Vector3 res;
    mAngleZ += 0x100;
    // Widening the comparison to int preserves the retail branch sequence.
    int suppressed = (mFlags & 8) != 0;
    if (suppressed)
        return 1;
    for (i = 0; i < 4; i++) {
        if (mMountedActorIds[i]) {
            dActor_c *rider = dActor_c::FindWithID(mMountedActorIds[i]);
            if (rider) {
                mid.x = 0;
                mid.y = 0;
                mid.z = 0;
                Matrix4x3_FromRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
                MulVec3Mat4x3(&data_ov002_0210ddd0[i], &data_020a0e68, &mid);
                Vec3_Add(&res, &mPos, &mid);
                rider->mPosX = res.x;
                rider->mPosY = res.y;
                rider->mPosZ = res.z;
            }
        }
    }
    UpdateModelTransform();
    if (IsClsnInRange(0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN17daObjKurumajiku_c17InitResourcesFromEPNS_9ResourcesEj
/* The shared setup both descendants' InitResources call: load the model and
   the collision mesh from the descendant's resource record, then spawn four
   riders of the given actor id at the axle and keep their uniqueIDs in
   mMountedActorIds for Behavior to carry. */
#pragma push
#pragma opt_strength_reduction off
int daObjKurumajiku_c::InitResourcesFrom(Resources *resources, unsigned int riderActorID) {
    int i;
    void *file;

    file = dExtSimpleModel_c::LoadFile(*resources->model);
    mModel.SetFile(static_cast<BMD_File *>(file), 1, -1);
    UpdateModelTransform();
    UpdateClsnPosAndRot();
    file = dBgW_Kc::LoadFile(*resources->collision);
    mMeshCollider.SetFile((KCL_File *)file, mClsnMat, 0x199, mAngleY, *resources->clps);
    for (i = 0; i < 4; i++) {
        mMountedActorIds[i] = 0;
        dActor_c *rider = dActor_c::Spawn(riderActorID, 0, mPos, 0, mAreaId, -1);
        if (rider != 0)
            mMountedActorIds[i] = rider->uniqueID;
    }
    return 1;
}
#pragma pop
