//cpp
#include "types.h"
// @symbol _ZN15TtcRotatingCube8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "TtcRotatingCube.h"
extern "C" {
extern u16 DecIfAbove0_Short(u16 *p);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *v);
extern int _Z14ApproachLinearRsss(s16 *val, int target, int step);
extern int RandomIntInternal(int *seed);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
}

extern u8 data_0209f2c0;
extern int data_0209e650;

int TtcRotatingCube::Behavior()
{
    if (data_0209f2c0 != 3) {
        switch (mState) {
        case 0:
            if (DecIfAbove0_Short(&mWaitTimer) != 0)
                break;
            _ZN5Sound9PlayBank3EjRK7Vector3(0x5b, &mCamSpacePosX);
            mState++;
            mVertSpeed = -0x5000;
            break;
        case 1:
            mVertSpeed += 0x800;
            mOffsetY += mVertSpeed;
            if (mOffsetY < 0)
                break;
            mOffsetY = 0;
            mWaitTimer = 6;
            mState++;
            break;
        case 2:
            if (DecIfAbove0_Short(&mWaitTimer) != 0)
                break;
            if (_Z14ApproachLinearRsss(&mAngleZ, mTargetAngleZ, 0x4b0) == 0)
                break;
            _ZN5Sound9PlayBank3EjRK7Vector3(0x40, &mCamSpacePosX);
            mState = 0;
            mWaitTimer = data_ov065_0211cfa4[data_0209f2c0];
            if (data_0209f2c0 == 2)
                mWaitTimer = (unsigned int)RandomIntInternal(&data_0209e650) % 7 * 0x14 + 5;
            mTargetAngleZ += data_ov065_0211cfa8[mVariant];
            break;
        }
    }
    UpdateModel();
    UpdateShadow();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsn();
    return 1;
}
