//cpp
// @symbol _ZN10dCapIcon_c6UnlinkEv
/* Unlinks this cap icon from its per-character list and restores the detached
 * defaults. The role is ROM-proven; the member spelling `Unlink` is inferred. */
#include "CapIcon.h"

extern dActor_c *data_0209f3e8[];
extern dCapIcon_c *data_ov001_020ad634[];
extern u8 data_ov001_020ad630[];

extern "C" void func_ov001_020aa6b0(dCapIcon_c *icon, int value);
extern "C" void func_ov001_020aa6cc(int character);

void dCapIcon_c::Unlink()
{
    if (((u32)mFlags << 29) >> 31)
        return;

    if (mSlot != -1) {
        if (data_0209f3e8[mSlot] == mOwner) {
            func_ov001_020aa6b0(this, 0);
            data_0209f3e8[mSlot] = 0;
            func_ov001_020aa6cc(mCharacter);
        }
    }

    if (mPrev)
        mPrev->mNext = mNext;
    else if (data_ov001_020ad634[mCharacter] == this)
        data_ov001_020ad634[mCharacter] = mNext;

    if (mNext)
        mNext->mPrev = mPrev;

    func_ov001_020aa6b0(this, 0);
    mOwner = 0;
    mOwnerUniqueID = 0;
    mPrev = 0;
    mNext = 0;
    mSlot = -1;
    mFlags = 0;
    mFlags |= 4;
    mCharacter = 3;
    unk_19 = 0;

    if (data_ov001_020ad630[mCharacter])
        --data_ov001_020ad630[mCharacter];
}
