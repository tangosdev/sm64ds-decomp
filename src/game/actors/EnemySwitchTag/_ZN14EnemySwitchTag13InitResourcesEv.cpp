//cpp
// @symbol _ZN14EnemySwitchTag13InitResourcesEv
/* EnemySwitchTag::InitResources -- vtable slot 0. Real C++ method over the
 * shared header; the pre-0x108 fields the body reads (actor flags at +0x8,
 * spawn params at +0x8c/+0x8e/+0x90) live in dActor_c's inherited span, read
 * by raw offset, and the named tail fields are written through members. */
#include "EnemySwitchTag.h"
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *clsn, void *a, int x, int z, unsigned int b, unsigned int c);
extern "C" void _ZN5Event8ClearBitEj(unsigned int bit);

int EnemySwitchTag::InitResources()
{
    char *a = (char *)this;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        a + 0xd4, this,
        ((*(s16 *)(a + 0x8c) + 1) * 0x64) << 0xc,
        ((*(s16 *)(a + 0x8e) + 1) * 0xc8) << 0xc,
        2, 0x400000);

    mEventID = *(u32 *)(a + 8) & 0x1f;
    mIsReusable = (*(u32 *)(a + 8) >> 5) & 1;

    {
        s16 t = *(s16 *)(a + 0x90);
        if (t <= 0)
            mHoldDuration = 0x96;
        else
            mHoldDuration = t;
    }
    mHoldTimer = 0;

    _ZN5Event8ClearBitEj(mEventID);
    return 1;
}
