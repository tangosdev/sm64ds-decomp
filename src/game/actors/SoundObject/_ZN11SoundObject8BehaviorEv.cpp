//cpp
// @symbol _ZN11SoundObject8BehaviorEv

#include "decl_common.h"
#include "SoundObject.h"

struct SoundObjectCallbackOwner;
typedef int (SoundObjectCallbackOwner::*SoundObjectCallback)(void *);
extern SoundObjectCallback data_ov002_0211110c[];

extern "C" {
void _ZN5Sound7PlaySubEjjj5Fix12IiEb(
    u32 a, u32 b, u32 d, Fix12i f, bool g);
}

int SoundObject::Behavior()
{
    SoundObjectCallbackOwner *owner = (SoundObjectCallbackOwner *)this;
    int result = (owner->*data_ov002_0211110c[param1])(&mTimer);
    if (result == 0 && mLevelID == data_0208e430
        && (mTimer <= 0xa || data_0209b49c > 0x7f)) {
        goto skip;
    }
    MarkForDestruction();
    if (data_0208e430 != 0x22) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(data_0208e430, 0x7f, 0, 0x7f000, 0);
    }
skip:
    if (param1 != 6) {
        if (data_0209b490 < mTimerThreshold)
            mTimer = mTimerReset;
    }
    return 1;
}
