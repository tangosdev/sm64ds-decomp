//cpp
// @symbol _ZN10DonutBlock8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "DonutBlock.h"
#include "dBgW.h"
extern "C" {
void _ZN8dActor_c9UpdatePosEP5dCc_c(void* thiz, void* clsn);
void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
int _ZNK10dBgCh_Actr10IsOnGroundEv(void* p);
int _ZN8dActor_c13DistToCPlayerEv(void* p);
void _ZN8dActor_c14TriplePoofDustEv(void* p);
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void* p);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void* p, int a, int b);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* p);
}

int DonutBlock::Behavior()
{
    switch (mState) {
    case 0:
        if (mHadClsn == 0) {
            mClsnTimer = 0;
        } else {
            unsigned char* pc9 = (unsigned char*)(((int)((char*)this) + 0x4e9));
            *pc9 = *pc9 + 1;
            mHadClsn = 0;
        }
        if (mClsnTimer >= 0xf) mState = 1;
        break;
    case 1:
        _ZN8dActor_c9UpdatePosEP5dCc_c(((char*)this), 0);
        dBgCh_Actr_UpdateContinuous_Veneer((char*)&mWithMeshClsn);
        if (_ZNK10dBgCh_Actr10IsOnGroundEv((char*)&mWithMeshClsn) == 0) {
            if (_ZN8dActor_c13DistToCPlayerEv(((char*)this)) <= 0x9c4000) break;
        }
        _ZN8dActor_c14TriplePoofDustEv(((char*)this));
        if (((dBgW *)((char*)&(*(u8 *)&mMeshCollider)))->IsEnabled() != 0) ((dBgW *)((char*)&(*(u8 *)&mMeshCollider)))->Disable();
        mPosX = mHomePosX;
        mPosY = mHomePosY;
        mPosZ = mHomePosZ;
        mState = 2;
        break;
    case 2: {
        int d = _ZN8dActor_c13DistToCPlayerEv(((char*)this));
        if (d <= 0x3e8000) break;
        if (d < 0x7d0000) {
            mVertSpeed = 0;
            mClsnTimer = 0;
            mHadClsn = 0;
            mState = 0;
        }
        break;
    }
    }
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
    if (mState != 2) {
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0x5dc000, 0) != 0) {
            _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
        }
    }
    return 1;
}
