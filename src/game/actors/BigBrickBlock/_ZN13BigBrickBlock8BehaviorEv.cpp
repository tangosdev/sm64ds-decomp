//cpp
// @symbol _ZN13BigBrickBlock8BehaviorEv
#include "BigBrickBlock.h"
#include "StarSwitch.h"

/* Event has no shared namespace declaration yet. The two unnamed collision
   setters remain address-named, and IsClsnInRangeOnScreen's by-value Fix12
   signature is a documented compiler wall, so those ABI calls stay explicit. */
extern "C" int _ZN5Event6GetBitEj(unsigned int a);
extern "C" void func_020393a4(void *p, int v);
extern "C" void func_02039394(void *p, int v);
extern "C" void _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);

int BigBrickBlock::Behavior()
{
    int is13 = (int)(actorID == 0x13);
    if (is13 != 0) {
        if (mPrevEventBit != _ZN5Event6GetBitEj(mEventID))
            mBroken = 0;

        if (mSwitch == 0) {
            unsigned int id = 0xb;
            do {
                mSwitch = (StarSwitch *)FindWithActorID(id, mSwitch);
            } while (mSwitch == 0 || mEventID != mSwitch->mEventBit);
        }

        if (_ZN5Event6GetBitEj(mEventID) == 0 || mBroken != 0) {
            if (mMeshCollider.IsEnabled() != 0)
                mMeshCollider.Disable();
        } else {
            func_020393a4(&mMeshCollider, 0x15e000);
            func_02039394(&mMeshCollider, 0x64000);
            _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0x150000, 0);
        }

        mPrevEventBit = _ZN5Event6GetBitEj(mEventID);
    } else {
        int v1 = 0x15e000;
        int v5 = 0x64000;
        int t;
        if ((t = (int)(actorID == 0x10)) != 0 ||
            (t = (int)(actorID == 0x11)) != 0 ||
            (t = (int)(actorID == 0x2e)) != 0) {
            v1 = 0x1c2000;
            v5 = 0x96000;
        }
        func_020393a4(&mMeshCollider, v1);
        func_02039394(&mMeshCollider, v5);
        _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0x600000, 0);
    }
    return 1;
}
