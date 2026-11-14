//cpp
/* Enemy creation and event-switch actors. The cartridge RTTI names both
 * classes; their adjacent, interleaved methods support this combined TU.
 * The original source boundary remains an inference.
 *
 * daECreate_c spawns actor 0xce when its event bit rises and hands it to
 * daBmb_c::func_ov102_0214ad14, the Bob-omb's chase start (src/game/actors/
 * d_a_bmb.cpp).
 * data_ov002_0210d9e0 is the ov002 model handle daECreate_c loads.
 */
#include "EnemySwitchTag.h"
#include "EnemySpawner.h"
#include "SharedFilePtr.h"
#include "dExtSimpleModel_c.h"
#include "Event.h"
#include "daBmb_c.h"

extern SharedFilePtr data_ov002_0210d9e0;

daESwitch_c *daESwitch_c_classInit() {
    return new daESwitch_c;
}

daECreate_c *daECreate_c_classInit() {
    return new daECreate_c;
}

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

int daECreate_c::InitResources() {
    mActorToSpawn = 0xce;
    mEventBit = mAngleZ & 0x1f;
    mPreviousEventBit = Event::GetBit(mEventBit);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9e0);
    return 1;
}

int daECreate_c::CleanupResources() {
    data_ov002_0210d9e0.Release();
    return 1;
}

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

int daECreate_c::Behavior() {
    if (Event::GetBit(mEventBit) && mPreviousEventBit == 0) {
        daBmb_c *actor =
            (daBmb_c *)dActor_c::Spawn(mActorToSpawn, 4, mPos, &mPrevAngle, mAreaId, -1);
        if (actor != 0)
            actor->func_ov102_0214ad14();
    }
    mPreviousEventBit = Event::GetBit(mEventBit);
    return 1;
}

int daESwitch_c::CleanupResources() {
    Event::ClearBit(mEventID);
    return 1;
}
