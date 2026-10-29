/* daESwitch_c is the cartridge RTTI name at ov002:0x0210b314.
 * Its typeinfo at 0x0210b2ec identifies dActor_c as the base. The vtable
 * address point is 0x0210b3e8, with overrides at slots 0/3/6 and the
 * destructor pair at 16/17. Field names are reconstructed from behavior.
 */
#ifndef ENEMYSWITCHTAG_H
#define ENEMYSWITCHTAG_H
#include "types.h"
#include "dActor_c.h"
#include "dCcAc_c.h"

struct daESwitch_c : dActor_c {
    u8 pad_0d0[0x4];
    /* dCcAc_c member. The cartridge's own ~daESwitch_c calls _ZN7dCcAc_cD1Ev at
       +0x0d4 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dCcAc_c mdCcAc_c; /* 0x0d4 */
    /* The tag sets an Event bit while something stands in its collider.
       mHoldDuration comes from the spawn rotation Z word (mAngleZ slot), or
       0x96 (150 frames) when that is not positive; mHoldTimer counts it down
       and clears both the collider flag and the Event bit when it runs out.
       mIsReusable is bit 5 of param1: set, the tag re-arms by reloading
       mHoldTimer from mHoldDuration; clear, it destroys itself after firing
       once. See daESwitch_c::InitResources and ::Behavior. */
    u16 mHoldDuration; /* 0x108 */
    u16 mHoldTimer;    /* 0x10a */
    u8 mIsReusable;    /* 0x10c */
    u8 mEventID;       /* 0x10d */

    virtual ~daESwitch_c() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char EnemySwitchTag_size_must_be_0x110[sizeof(daESwitch_c) == 0x110 ? 1 : -1];
#endif

typedef daESwitch_c EnemySwitchTag;

#endif
