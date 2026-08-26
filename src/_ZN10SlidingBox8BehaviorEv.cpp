//cpp
// @symbol _ZN10SlidingBox8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "SlidingBox.h"
#include "SurfaceInfo.h"

extern short data_02082214[];

namespace cstd { int fdiv(int a, int b); }
namespace Sound {
u32 PlayLong(u32 handle, u32 player, u32 soundID,
             const Vector3 &position, s16 arg);
}

extern "C" {
void dBgCh_Actr_UpdateContinuous_Veneer(void *p);
void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
}

int SlidingBox::Behavior()
{
    Vector3 normal;

    switch (mState) {
    case 0:
        mShip = dActor_c::FindWithActorID(0x39, 0);
        if (mShip == 0) {
            MarkForDestruction();
            break;
        }
        mState++;
        /* fallthrough */
    case 1:
        UpdatePos(0);
        dBgCh_Actr_UpdateContinuous_Veneer((char *)&mWithMeshClsn);
        if (mWithMeshClsn.IsOnGround()) {
            mState++;
            mBasePos.x = mPosX;
            mBasePos.y = mPosY;
            mBasePos.z = mPosZ;
        }
        break;
    case 2: {
        s16 *shipAngles = &mShip->mAngleX;
        int spd;
        mAngleX = shipAngles[0];
        mAngleY = shipAngles[1];
        mAngleZ = shipAngles[2];
        mPrevAngleY = mAngleY;
        mHorzSpeed = data_02082214[((u16)mAngleX >> 4) * 2] * 0x8c;
        mHorzPos += mHorzSpeed;
        spd = mHorzPos;
        if (spd >= 0x4ff000)
            mHorzPos = 0x4ff000;
        else if (spd < -0x32000)
            mHorzPos = -0x32000;
        mPosX = mBasePos.x + (int)(((long long)mHorzPos * data_02082214[((u16)mAngleY >> 4) * 2] + 0x800) >> 12);
        mPosZ = mBasePos.z + (int)(((long long)mHorzPos * data_02082214[((u16)mAngleY >> 4) * 2 + 1] + 0x800) >> 12);
        mHorzSpeed = 0;
        UpdatePos(0);
        dBgCh_Actr_UpdateContinuous_Veneer((char *)&mWithMeshClsn);
        if (mWithMeshClsn.IsOnGround()) {
            void *fr = _ZNK10dBgCh_Actr14GetFloorResultEv((char *)&mWithMeshClsn);
            ((SurfaceInfo *)((char *)fr + 4))->CopyNormalTo(normal);
            if (normal.y != 0) {
                mVertSpeed = -(cstd::fdiv(
                    (int)(((long long)normal.x * unk_0a4 + 0x800) >> 12)
                  + (int)(((long long)normal.z * unk_0ac + 0x800) >> 12),
                    normal.y) + 0x8000);
            }
        }
        if (DistToCPlayer() < 0x7d0000) {
            int vel = mHorzSpeed;
            if (vel < 0)
                vel = -vel;
            if (vel > 0x3000) {
                mSoundID = Sound::PlayLong(
                    mSoundID, 3, 0x9f, *(Vector3 *)&mCamSpacePosX, 0);
            }
        }
        break;
    }
    }

    UpdateModel();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_((char *)this, 0, 0)) {
        UpdateClsnPosAndRot();
    }
    return 1;
}
