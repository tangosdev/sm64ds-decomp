//cpp
// @symbol _ZN15daObjPathLift_c8BehaviorEv
/* daObjPathLift_c::Behavior -- vtable slot 4. Runs PathLift's shared step,
 * updates travel audio/model/collision state, and keeps the collision mesh
 * enabled in debug mode. Real C++ method over the recovered hierarchy. */
#include "daObjPathLift_c.h"

extern "C" {
s32 Vec3_Dist(const Vector3 *a, const Vector3 *b);
unsigned char DecIfAbove0_Byte(unsigned char *value);
void func_020393a4(void *collider, int value);
void func_02039394(void *collider, int value);
void func_ov002_020efcf4(daObjPathLift_c *self);
int func_ov100_0214700c(daObjPathLift_c *self);
void func_ov100_02146e70(daObjPathLift_c *self);

/* The true signature takes two by-value Fix12<int> parameters. A faithful
 * member call homes them to the stack, unlike the ROM, so retain the verified
 * scalar ABI spelling (runbook wall 6az). */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
}
namespace Sound {
unsigned int PlayLong(unsigned int handle, unsigned int arg1, unsigned int soundID,
                      const Vector3 &pos, short arg4);
}
extern unsigned char data_0209f2d8;

int daObjPathLift_c::Behavior()
{
    func_ov002_020efcf4(this);
    BaseBehavior();
    if (Vec3_Dist((Vector3 *)&mPosX, (Vector3 *)&mPrevPosX) != 0) {
        if (DecIfAbove0_Byte(&unk_4b0) == 0) {
            unk_4a8 = Sound::PlayLong(
                unk_4a8, 3, 0x82, *(Vector3 *)&mCamSpacePosX, 0);
        }
    }

    func_ov100_0214700c(this);
    UpdateClsnPosAndRot();
    func_ov100_02146e70(this);
    func_020393a4(&mMeshCollider, 0x150000);
    func_02039394(&mMeshCollider, 0x1000);

    int debugMode = (int)(data_0209f2d8 == 1);
    if (debugMode != 0) {
        if (mMeshCollider.IsEnabled() == 0)
            mMeshCollider.Enable(this);
    } else {
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    }
    return 1;
}
