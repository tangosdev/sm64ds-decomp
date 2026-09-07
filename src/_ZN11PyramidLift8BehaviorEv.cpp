//cpp
// @symbol _ZN11PyramidLift8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "PyramidLift.h"
extern "C" {
extern short data_02082214[];
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
}

int PyramidLift::Behavior()
{
    switch (mState) {
    case 0:
        if (mHadClsn != 0) {
            mState = 1;
            mShakeTimer = 0;
        }
        break;
    case 1: {
        unsigned short ang = mShakeTimer;
        int t = (int)((unsigned short)((int)(ang << 0x1c) >> 0x10));
        int idx = t >> 4;
        int s = *(short*)((char*)data_02082214 + (idx << 2));
        int d = (int)(((long long)s * 0xa + 0x800) >> 0xc);
        mPosY = mBasePosY + d;
        if (mShakeTimer == 8) {
            mState = 2;
            mVertSpeed = -0xa000;
        }
        {
            unsigned short *pa = (unsigned short*)(((int)((char*)this) + 0x3f4));
            *pa = *pa + 1;
        }
        break;
    }
    case 2: {
        int v = mPosY;
        int idx = mNextBullet;
        int* p = (int*)(((char*)this) + idx * 0xc + 0x380);
        int lim = *p + 0x14000;
        if (v <= lim) {
            unsigned char *pb = (unsigned char*)(((int)((char*)this) + 0x3f8));
            *pb = *pb + 1;
        }
        {
            int *py = (int*)(((int)((char*)this) + 0x60));
            *py = *py + mVertSpeed;
        }
        if (mPosY < 0x80000) {
            mPosY = 0x80000;
            mState = 3;
            mShakeTimer = 0;
        }
        break;
    }
    case 3: {
        int z = 0;
        unsigned short ang = mShakeTimer;
        int t = (int)((unsigned short)((int)(ang << 0x1c) >> 0x10));
        int idx = t >> 4;
        int s = *(short*)((char*)data_02082214 + (idx << 2));
        int d = (int)(((long long)s * 0xa + 0x800) >> 0xc);
        mPosY = d + 0x80000;
        {
            unsigned short *pa = (unsigned short*)(((int)((char*)this) + 0x3f4));
            if (mShakeTimer >= 8) {
                mVertSpeed = z;
                mPosY = 0x80000;
            }
            *pa = *pa + 1;
        }
        break;
    }
    }
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0))
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
    mHadClsn = 0;
    return 1;
}
