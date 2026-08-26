//cpp
// @symbol _ZN7Tornado6State0Ev
#include "Tornado.h"

extern "C" u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(
    u32 handle, u32 bank, u32 sound, const void *pos, u32 flags);

void Tornado::State0()
{
    if (mChaseTimer >= 0x384) {
        mPosX = mHomePosX;
        mPosY = mHomePosY;
        mPosZ = mHomePosZ;
    }

    if (mTriggerCount == 0) {
        mParticleHandle1 = 0;
        mParticleHandle0 = mParticleHandle1;
        mScaleX = 0;
        mScaleY = 0;
        mScaleZ = 0;
        unk_352 = 0;
        if (DistToCPlayer() < 0x5dc000) {
            u8 *trigger = &mTriggerCount;
            *trigger = *trigger + 1;
        }
        mStateTimer = 0;
        mdCcAc_c.flags &= ~1;
        mChaseTimer = 0;
    } else {
        mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
            mSoundHandle, 3, 0x85, &mCamSpacePosX, 0);
        s32 timer = mStateTimer;
        UpdateSpin((timer << 12) / 60);
        if (mStateTimer >= 0x3c)
            mState = 1;
    }
}
