//cpp
/* Rickshaw axle shared by the daKpa_c in the Dark World and daKpa_c in the Sky
 * actors. It turns the model and carries up to four mounted carts.
 *
 * This TU owns ov002 .text 0x020b69e4..0x020b6d28: the two destructors (D0
 * 0x020b69e4, D1 0x020b6a3c) and the five functions below, in ROM order under
 * `#pragma defer_codegen off`, ending with the shared setup helper
 * func_ov002_020b6c54; the discarded class metadata is recorded in the manifest.
 *
 * deslop
 * Leftover: (Vector3 *)&mPosX -- no Pos() accessor exists on this branch,
 *   so the pun stays.
 * Leftover: data_ov002_0210ddd0 (four rider mount offsets) and
 *   data_020a0e68 (shared arm9 matrix scratch, used tree-wide) keep
 *   linker names; naming belongs at a shared home, not this leaf.
 * Leftover: func_ov002_020b6a80 keeps its char* entry contract, shared
 *   with the existing setup helper; the C boundary in func_ov002_020b6ac8
 *   stays opaque (both descendants retain their own pointer views).
 */

#pragma defer_codegen off

#include "daObjKurumajiku_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

typedef char AxleResources_size_must_be_0x0c[
    sizeof(daObjKurumajiku_c::Resources) == 0x0c ? 1 : -1];

extern "C" {
extern Vector3 data_ov002_0210ddd0[4];
extern char data_020a0e68[0x30];

void Matrix4x3_FromRotationZXYExt(void *dst, int x, int y, int z);
void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void MulVec3Mat4x3(const Vector3 *v, const void *m, Vector3 *dst);
void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);

int func_ov002_020b6a80(char *self);
int func_ov002_020b6ac8(void *actor, void *descriptor);

/* Matches the scalar definition in dBgActor_c.cpp. Its encoded Fix12 name
   does not establish a by-value class parameter; see dBgActor_c.h. */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int radius, int yOffset);
// local extern: the actual free definition accepts a scalar scale; the
// member call takes Fix12<int> by value (wall 6az).
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *file, const Matrix4x3 *matrix, int scale, s16 angle, void *clps);
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
void daObjKurumajiku_c_EmitDeletingDestructor(daObjKurumajiku_c *p)
{
    delete p;
}

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjKurumajiku_c_EmitDestructor(daObjKurumajiku_c *p)
{
    p->~daObjKurumajiku_c();
}

// @symbol func_ov002_020b6a80
/* Build the model transform. Model translation uses 1/8 of the world position.
   The char* entry contract is shared with the existing setup helper. */
extern "C" int func_ov002_020b6a80(char *self)
{
    daObjKurumajiku_c *actor = reinterpret_cast<daObjKurumajiku_c *>(self);
    Matrix4x3_FromRotationZXYExt(&actor->mModel.mat4x3, 0, actor->mAngleY, actor->mAngleZ);
    actor->mModel.mat4x3.m[9] = actor->mPosX >> 3;
    actor->mModel.mat4x3.m[10] = actor->mPosY >> 3;
    int z = actor->mPosZ >> 3;
    actor->mModel.mat4x3.m[11] = z;
    return z;
}

// @symbol func_ov002_020b6ac8
/* Disable the moving collision before releasing the model and collision files.
   The C boundary remains opaque: both descendants retain their own pointer views. */
extern "C" int func_ov002_020b6ac8(void *actor, void *descriptor)
{
    daObjKurumajiku_c *self = static_cast<daObjKurumajiku_c *>(actor);
    daObjKurumajiku_c::Resources *resources =
        static_cast<daObjKurumajiku_c::Resources *>(descriptor);
    if (self->mMeshCollider.IsEnabled())
        self->mMeshCollider.Disable();
    resources->model->Release();
    resources->collision->Release();
    return 1;
}

// @symbol _ZN17daObjKurumajiku_c6RenderEv
s32 daObjKurumajiku_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN17daObjKurumajiku_c8BehaviorEv
/* The angle advances even while bit 3 suppresses rider and collision updates. */
s32 daObjKurumajiku_c::Behavior()
{
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
                Matrix4x3_FromRotationXYZExt(data_020a0e68, mAngleX, mAngleY, mAngleZ);
                MulVec3Mat4x3(&data_ov002_0210ddd0[i], data_020a0e68, &mid);
                Vec3_Add(&res, (Vector3 *)&mPosX, &mid);
                rider->mPosX = res.x;
                rider->mPosY = res.y;
                rider->mPosZ = res.z;
            }
        }
    }
    func_ov002_020b6a80((char *)this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol func_ov002_020b6c54
/* The shared setup helper both descendants' InitResources call: load the
   model and the collision mesh from the descendant's resource record, then
   spawn four riders of the given actor id at the axle and keep their
   uniqueIDs in mMountedActorIds for Behavior to carry. Like its cleanup
   sibling it keeps an opaque C boundary. */
#pragma push
#pragma opt_strength_reduction off
extern "C" int func_ov002_020b6c54(void *actor, void *descriptor, unsigned int riderActorID)
{
    int i;
    daObjKurumajiku_c *self = static_cast<daObjKurumajiku_c *>(actor);
    daObjKurumajiku_c::Resources *resources =
        static_cast<daObjKurumajiku_c::Resources *>(descriptor);
    void *file;

    file = Model::LoadFile(*resources->model);
    self->mModel.SetFile(static_cast<BMD_File *>(file), 1, -1);
    func_ov002_020b6a80(reinterpret_cast<char *>(self));
    self->UpdateClsnPosAndRot();
    file = dBgW_Kc::LoadFile(*resources->collision);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &self->mMeshCollider, file, &self->mClsnMat, 0x199, self->mAngleY,
        resources->clps);
    for (i = 0; i < 4; i++) {
        self->mMountedActorIds[i] = 0;
        dActor_c *rider = dActor_c::Spawn(riderActorID, 0,
            *reinterpret_cast<const Vector3 *>(&self->mPosX), 0, self->mAreaId, -1);
        if (rider != 0)
            self->mMountedActorIds[i] = rider->uniqueID;
    }
    return 1;
}
#pragma pop
