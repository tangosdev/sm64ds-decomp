//cpp
// @symbol _ZN6Cannon13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Cannon.h"
#include "SharedFilePtr.h"

/* Fix12<T> by-value parameters are a proven CodeWarrior source wall, so this
 * one ABI seam stays explicit; the receiver and actor are nevertheless typed. */
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *self, dActor_c *actor, int radius, int height,
    u32 flags, u32 vulnFlags);
extern "C" void func_ov098_0213b15c(Cannon *cannon);

extern SharedFilePtr data_ov098_0213c8e8;
extern SharedFilePtr data_ov098_0213c91c;
extern SharedFilePtr data_ov002_0210da38;

int Cannon::InitResources()
{
    BMD_File *file = (BMD_File *)Model::LoadFile(data_ov098_0213c8e8);
    mModel.SetFile(file, 1, -1);
    unk_194 = *(s32 *)((u8 *)mModel.data.bones + 0x58);
    mVariant = param1 & 3;
    unk_174 = 0;
    mPosY -= 0x50000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mHomeAngleY = mPrevAngleY;
    mFireStep = 0;

    if (mVariant != 2) {
        if (mVariant == 1)
            mAngleX = 0x2000;
        mAimTimer = 0x2000;
        mTargetAngleY = mAngleY;
        mState = 0;
        func_ov098_0213b15c(this);
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
            &mdCcAc_c, this, 0xa0000, 0x12c000, 0x800004, 0);
    } else {
        mPosY -= 0x190000;
        mTargetAngleY = mAngleZ;
        mAimTimer = 0x2000;
        mState = 2;
        mFlags &= ~1;
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
            &mdCcAc_c, this, 0x50000, 0x12c000, 0x800004, 0);
    }

    Model::LoadFile(data_ov098_0213c91c);
    Model::LoadFile(data_ov002_0210da38);
    return 1;
}
