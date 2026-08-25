//cpp
// @symbol _ZN10SlidingBox8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "SlidingBox.h"

extern short data_02082214[];

extern "C" {
void *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void *p);
void _ZN7fBase_c18MarkForDestructionEv(void *self);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
void dBgCh_Actr_UpdateContinuous_Veneer(void *p);
int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *self, Vector3 *out);
int _ZN4cstd4fdivEii(int a, int b);
int _ZN8dActor_c13DistToCPlayerEv(void *self);
int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int, unsigned int, unsigned int, void *, unsigned int);
void func_ov016_021130a4(char *t);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
}

int SlidingBox::Behavior()
{
    Vector3 normal;

    switch (mState) {
    case 0:
        *(void **)&mShip = _ZN8dActor_c15FindWithActorIDEjPS_(0x39, 0);
        if (*(void **)&mShip == 0) {
            _ZN7fBase_c18MarkForDestructionEv((char *)this);
            break;
        }
        mState++;
        /* fallthrough */
    case 1:
        _ZN8dActor_c9UpdatePosEP5dCc_c((char *)this, 0);
        dBgCh_Actr_UpdateContinuous_Veneer((char *)&mWithMeshClsn);
        if (_ZNK10dBgCh_Actr10IsOnGroundEv((char *)&mWithMeshClsn)) {
            mState++;
            unk_4e0 = mPosX;
            unk_4e4 = mPosY;
            unk_4e8 = mPosZ;
        }
        break;
    case 2: {
        s16 *src = (s16 *)((*(char **)&mShip) + 0x8c);
        int spd;
        mAngleX = src[0];
        mAngleY = src[1];
        mAngleZ = src[2];
        mPrevAngleY = mAngleY;
        mHorzSpeed = data_02082214[((u16)mAngleX >> 4) * 2] * 0x8c;
        mHorzPos += mHorzSpeed;
        spd = mHorzPos;
        if (spd >= 0x4ff000)
            mHorzPos = 0x4ff000;
        else if (spd < -0x32000)
            mHorzPos = -0x32000;
        mPosX = unk_4e0 + (int)(((long long)mHorzPos * data_02082214[((u16)mAngleY >> 4) * 2] + 0x800) >> 12);
        mPosZ = unk_4e8 + (int)(((long long)mHorzPos * data_02082214[((u16)mAngleY >> 4) * 2 + 1] + 0x800) >> 12);
        mHorzSpeed = 0;
        _ZN8dActor_c9UpdatePosEP5dCc_c((char *)this, 0);
        dBgCh_Actr_UpdateContinuous_Veneer((char *)&mWithMeshClsn);
        if (_ZNK10dBgCh_Actr10IsOnGroundEv((char *)&mWithMeshClsn)) {
            void *fr = _ZNK10dBgCh_Actr14GetFloorResultEv((char *)&mWithMeshClsn);
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)fr + 4, &normal);
            if (normal.y != 0) {
                mVertSpeed = -(_ZN4cstd4fdivEii(
                    (int)(((long long)normal.x * unk_0a4 + 0x800) >> 12)
                  + (int)(((long long)normal.z * unk_0ac + 0x800) >> 12),
                    normal.y) + 0x8000);
            }
        }
        if (_ZN8dActor_c13DistToCPlayerEv((char *)this) < 0x7d0000) {
            int vel = mHorzSpeed;
            if (vel < 0)
                vel = -vel;
            if (vel > 0x3000) {
                mSoundID = _ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundID, 3, 0x9f, (char *)this + 0x74, 0);
            }
        }
        break;
    }
    }

    func_ov016_021130a4((char *)this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_((char *)this, 0, 0)) {
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv((char *)this);
    }
    return 1;
}
