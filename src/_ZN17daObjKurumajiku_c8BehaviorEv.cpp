//cpp
// @symbol _ZN17daObjKurumajiku_c8BehaviorEv
#include "daObjKurumajiku_c.h"
/* daObjKurumajiku_c::Behavior -- vtable slot 6, ov002 0x020b6b38.
 *
 * Named from the table: 0x020b6b38 is the word slot 6 of
 * _ZTV17daObjKurumajiku_c holds where _ZTV10dBgActor_c holds something else, so
 * it is this class's own override of the virtual fBase_c declares.
 * include/daObjKurumajiku_c.h's banner already described this function -- it
 * is where mMountedActorIds was named from -- so only the definition was
 * still under a func_ov002_ name.
 *
 * The axle spins 0x100 per frame and drags up to four mounted actors around
 * with it: each ride's offset comes out of the four-entry table at
 * data_ov002_0210ddd0, rotated by the axle's own Euler angles through the
 * scratch matrix at data_020a0e68 and added to the axle's position. Bit 8 of
 * mFlags suppresses all of it -- fBase_c's own "behaviour off" bit -- and
 * the early return skips the collision update too, which is what makes a
 * stopped axle stop carrying.
 *
 * The pre-migration file had to invent a flat `struct Obj` for all of this;
 * every field it reached by offset is one dActor_c or this class already names. */

extern "C" {
extern Vector3 data_ov002_0210ddd0[4];
extern char data_020a0e68[0x30];

void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void MulVec3Mat4x3(const Vector3 *v, const void *m, Vector3 *dst);
void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);
int  func_ov002_020b6a80(daObjKurumajiku_c *self);

/* Carries Fix12<int> by value, so its definition stays a mangled free
   function -- see include/ShadowModel.h. A call is unaffected. */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int x, int z);
}

s32 daObjKurumajiku_c::Behavior()
{
    int i;
    Vector3 mid;
    Vector3 res;
    s16 *rzp = &mAngleZ;

    *rzp = *rzp + 0x100;
    {
        int suppressed = (int)((mFlags & 8) != 0);
        if (suppressed != 0)
            return 1;
    }
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
    func_ov002_020b6a80(this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}
