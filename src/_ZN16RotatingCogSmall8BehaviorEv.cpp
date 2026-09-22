//cpp
// @symbol _ZN16RotatingCogSmall8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "RotatingCogSmall.h"
extern "C" {
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern int _Z14ApproachLinearRsss(short* dst, short to, short step);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern int RandomIntInternal(int* seed);
extern unsigned char data_0209f2c0[];
extern short data_ov035_02111ef4[][4];
extern short data_ov035_02111ef0[];
extern int data_0209e650[];
}

int RotatingCogSmall::Behavior()
{
    if (data_0209f2c0[0] == 3) {
        _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
        if (mRotationState == 0 && _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0))
            _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
        return 1;
    }

    if (_Z14ApproachLinearRsss((short*)((char*)&mAngleY), mTargetAngleY, 0xc8) != 0 &&
        DecIfAbove0_Short((unsigned short*)((char*)&mStepTimer)) == 0) {
        short* p = (short*)(((int)((char*)this) + 0x322));
        *p = *p + mAngleYStep;
        unsigned char k = data_0209f2c0[0];
        mStepTimer = data_ov035_02111ef4[mRotationState][k];
        if (k == 2) {
            int rnd = RandomIntInternal(data_0209e650);
            if (DecIfAbove0_Short((unsigned short*)((char*)&mDirTimer)) == 0) {
                if ((unsigned int)rnd % 3 != 0) {
                    int r2 = (rnd & 3) * 0x3c;
                    mAngleYStep = data_ov035_02111ef0[mRotationState];
                    mDirTimer = r2 + 0x5a;
                } else {
                    mAngleYStep = -data_ov035_02111ef0[mRotationState];
                    mDirTimer = ((unsigned int)rnd % 3 + 1) * 0x1e;
                }
            }
            mStepTimer = (unsigned int)rnd % 3 * 0x14 + 0xa;
        }
    }

    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
    if (mRotationState == 0 && _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0))
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
    return 1;
}
