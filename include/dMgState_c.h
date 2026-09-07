#ifndef DMGSTATE_C_H
#define DMGSTATE_C_H

#include "types.h"

/* Non-polymorphic state controller embedded in dScMgBase_c. The class spelling
 * is inferred from the minigame subsystem's dMg* naming; the 0x28-byte layout,
 * construction order, three CodeWarrior member-function callbacks, 20-state
 * table, and per-frame behavior/render roles are all ROM-proven. */
struct dMgState_c {
    typedef void (dMgState_c::*Callback)();

    Callback mEnter;
    Callback mBehavior;
    Callback mRender;
    s32 mState;
    s32 mTimer;
    s32 unk_020;
    s32 unk_024;

    dMgState_c();
    void Render();
    void Behavior();
    void SetState(s32 state);
};

typedef char dMgState_c_size_must_be_0x28[
    sizeof(dMgState_c) == 0x28 ? 1 : -1];

#endif
