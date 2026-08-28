//cpp
// @symbol _ZN14KnockDownPlank8BehaviorEv
#include "KnockDownPlank.h"
#include "Sound.h"

extern "C" {
extern short data_02082214[];
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(
    dActor_c *self, const Vector3 *pos, Fix12i strength);
s32 _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(
    dBgActor_c *self, Fix12i rangeOffset, Fix12i drawDistance);
void func_ov015_0211166c(KnockDownPlank *plank);
s32 func_ov015_021114f0(KnockDownPlank *plank);
}

int KnockDownPlank::Behavior()
{
    switch (mState) {
    case 0:
        break;
    case 1: {
        unsigned short ang = mWobbleAng;
        int idx = ang >> 4;
        int s = data_02082214[idx * 2];
        int amp = mKnockDir;
        int prod = amp * s;
        int d = (int)(((long long)prod * mWobbleTimer + 0x800) >> 0xc);
        mAngleX = d;
        if (mWobbleTimer <= 0) {
            mAngleX = 0;
            mWobbleAng = 0;
            mState = 0;
        } else {
            mWobbleTimer -= 8;
        }
        mWobbleAng += 0x400;
        break;
    }
    case 2: {
        int amt = mKnockDir << 0x17;
        mFallAngVel += amt >> 16;
        mAngleX += mFallAngVel;
        if (mKnockDir == -1) {
            if (mAngleX < -0x4000) {
                mAngleX = -0x4000;
                mFallAngVel = 0;
                mState++;

                Vector3 pos;
                pos.x = mPosX;
                pos.y = mPosY;
                pos.z = mPosZ;
                _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(
                    this, &pos, 0x2000000);
                Sound::PlayBank3(0x44, *(Vector3 *)&mCamSpacePosX);
            }
        } else {
            mJumpSpeed += 0x2000;
            mPosY += mJumpSpeed;
            int limit = mOriginalPosY + 0x46000;
            if (mPosY > limit) {
                mPosY = limit;
            }
            if (mAngleX > 0x4000) {
                mAngleX = 0x4000;
                mFallAngVel = 0;
                mState++;

                Vector3 pos;
                pos.x = mPosX;
                pos.y = mPosY;
                pos.z = mPosZ;
                _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(
                    this, &pos, 0x2000000);
                Sound::PlayBank3(0x44, *(Vector3 *)&mCamSpacePosX);
            }
        }
        break;
    }
    case 3:
        break;
    default:
        break;
    }

    func_ov015_0211166c(this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    func_ov015_021114f0(this);
    return 1;
}
