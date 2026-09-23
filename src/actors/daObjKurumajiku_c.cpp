//cpp
/* Rickshaw axle shared by the Bowser in the Dark World and Bowser in the Sky
 * actors. It turns the model and carries up to four mounted carts.
 *
 * This partial TU owns the four functions below. The two destructors retain
 * their individual source files; their emitted order and the discarded class
 * metadata are recorded in the manifest and the source-repair experiment.
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
}

/* mwccarm 2004/b56 emits one .text section per function in the REVERSE of
   source order, so the highest-address cartridge function is written first. */

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

// @symbol _ZN17daObjKurumajiku_c6RenderEv
s32 daObjKurumajiku_c::Render()
{
    mModel.Render(0);
    return 1;
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
