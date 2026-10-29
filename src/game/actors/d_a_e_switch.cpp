//cpp
/* Enemy creation and event-switch actors. The cartridge RTTI names both
 * classes; their adjacent, interleaved methods support this combined TU.
 * The original source boundary remains an inference.
 *
 * daECreate_c spawns actor 0xce when its event bit rises and hands it to
 * func_ov102_0214ad14, the daBmb_c chase-start helper (src/game/actors/
 * d_a_bmb.cpp), which keeps its label because three overlays call it.
 * data_ov002_0210d9e0 is the ov002 model handle daECreate_c loads.
 */
#include "EnemySwitchTag.h"
#include "EnemySpawner.h"
#include "SharedFilePtr.h"
#include "dExtSimpleModel_c.h"
#include "Event.h"

extern SharedFilePtr data_ov002_0210d9e0;
extern "C" void func_ov102_0214ad14(void *actor);

// @symbol daESwitch_c_classInit
extern "C" daESwitch_c *daESwitch_c_classInit() {
    return new daESwitch_c;
}

// @symbol daECreate_c_classInit
extern "C" daECreate_c *daECreate_c_classInit() {
    return new daECreate_c;
}

// @symbol _ZN11daESwitch_c13InitResourcesEv
int daESwitch_c::InitResources() {
    mdCcAc_c.Init(this, ((mAngleX + 1) * 100) << 12, ((mAngleY + 1) * 200) << 12, 2, 0x400000);
    mEventID = param1 & 0x1f;
    mIsReusable = (param1 >> 5) & 1;
    if (mAngleZ <= 0)
        mHoldDuration = 150;
    else
        mHoldDuration = mAngleZ;
    mHoldTimer = 0;
    Event::ClearBit(mEventID);
    return 1;
}

// @symbol _ZN11daECreate_c13InitResourcesEv
int daECreate_c::InitResources() {
    mActorToSpawn = 0xce;
    mEventBit = mAngleZ & 0x1f;
    mPreviousEventBit = Event::GetBit(mEventBit);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9e0);
    return 1;
}

// @symbol _ZN11daECreate_c16CleanupResourcesEv
int daECreate_c::CleanupResources() {
    data_ov002_0210d9e0.Release();
    return 1;
}

// @symbol _ZN11daESwitch_c8BehaviorEv
int daESwitch_c::Behavior() {
    if (mHoldTimer != 0) {
        --mHoldTimer;
        if (mHoldTimer == 0) {
            mdCcAc_c.flags &= ~1;
            Event::ClearBit(mEventID);
        }
    }
    if (mdCcAc_c.otherOwner != 0) {
        mdCcAc_c.flags |= 1;
        Event::SetBit(mEventID);
        if (mIsReusable != 0)
            mHoldTimer = mHoldDuration;
        else
            MarkForDestruction();
    }
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN11daECreate_c8BehaviorEv
int daECreate_c::Behavior() {
    if (Event::GetBit(mEventBit) && mPreviousEventBit == 0) {
        dActor_c *actor = dActor_c::Spawn(mActorToSpawn, 4, mPos, &mPrevAngle, mAreaId, -1);
        if (actor != 0)
            func_ov102_0214ad14(actor);
    }
    mPreviousEventBit = Event::GetBit(mEventBit);
    return 1;
}

// @symbol _ZN11daESwitch_c16CleanupResourcesEv
int daESwitch_c::CleanupResources() {
    Event::ClearBit(mEventID);
    return 1;
}
