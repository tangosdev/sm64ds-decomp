//cpp
#include "LavaPlank.h"

struct SinCosEntry { s16 sin, cos; };
extern "C" {
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *);
}
extern SinCosEntry data_02082214[];

int LavaPlank::Behavior()
{
    int val = (u16)mPhaseAngle >> 4;
    mPosY = data_02082214[val].sin * (s16)0x1e + mOriginalPosY;
    mPhaseAngle += 0x400;
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
    return 1;
}
