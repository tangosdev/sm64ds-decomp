//cpp
#include "types.h"
// @symbol _ZN8ShipWing8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ShipWing.h"
extern s16 data_02082214[];
extern "C" {
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void* v);
}

int ShipWing::Behavior()
{
    s16 a = mFrame << 10;
    if (mReverseMotion == 0) {
        int idx = ((u16)a >> 4) * 2;
        mAngleY = mBaseAngleY + (int)((((s64)data_02082214[idx] << 13) + 0x800) >> 12);
        mAngleX = mBaseAngleX + (int)((((s64)data_02082214[idx + 1] << 11) + 0x800) >> 12);
    } else {
        int idx = ((u16)a >> 4) * 2;
        mAngleY = mBaseAngleY - (int)((((s64)data_02082214[idx] << 13) + 0x800) >> 12);
        mAngleX = mBaseAngleX + (int)((((s64)data_02082214[idx + 1] << 11) + 0x800) >> 12);
    }
    {
        u16 *p = &mFrame;
        *p = *p + 1;
    }
    if (mPlaySound != 0) {
        if (mFrame == 0x40) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x75, &mCamSpacePosX);
            mFrame = 0;
        }
    }
    func_ov036_02111618(((char*)this));
    return 1;
}
