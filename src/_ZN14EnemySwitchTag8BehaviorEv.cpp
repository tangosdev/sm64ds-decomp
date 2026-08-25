//cpp
#include "types.h"
// @symbol _ZN14EnemySwitchTag8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "EnemySwitchTag.h"
extern "C" {
extern void _ZN5Event8ClearBitEj(u32 bit);
extern void _ZN5Event6SetBitEj(u32 bit);
extern void _ZN7fBase_c18MarkForDestructionEv(void* p);
extern void _ZN5dCc_c5ClearEv(void* p);
extern void _ZN5dCc_c6UpdateEv(void* p);
}

int EnemySwitchTag::Behavior()
{
    if (mHoldTimer != 0) {
        *(u16*)((int)((char*)this) + 0x10a) -= 1;
        if (mHoldTimer == 0) {
            *(u32*)((int)((char*)this) + 0xec) &= ~1;
            _ZN5Event8ClearBitEj(mEventID);
        }
    }
    /* Both of these are fields of the dCcAc_c at 0x0d4, which the cartridge's own
       ~EnemySwitchTag names (tools/dtor_members.py): 0x0f8 is +0x24, dCc_c::otherOwner,
       and 0x0ec is +0x18, dCc_c::flags. */
    if (mdCcAc_c.otherOwner != 0) {
        *(u32*)((char*)&mdCcAc_c.flags) |= 1;
        _ZN5Event6SetBitEj(mEventID);
        if (mIsReusable != 0) {
            mHoldTimer = mHoldDuration;
        } else {
            _ZN7fBase_c18MarkForDestructionEv(((char*)this));
        }
    }
    _ZN5dCc_c5ClearEv((char*)&mdCcAc_c);
    _ZN5dCc_c6UpdateEv((char*)&mdCcAc_c);
    return 1;
}
