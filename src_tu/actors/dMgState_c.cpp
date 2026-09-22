//cpp
/* dMgState_c -- the minigame scene's little state machine.
 *
 * dScMgBase_c keeps one of these by value and drives it once a frame: Behavior
 * counts the state timer down and calls the current state's per-frame handler,
 * Render calls the current state's draw handler, and SetState swaps both out.
 * Every handler is a pointer to a dScMgBase_c member, so the table below is a
 * table of member pointers rather than of plain function addresses.
 *
 * mwccarm emits one .text section per function and lays them out in the reverse
 * of source order, so the members are written here highest-ROM-address first.
 *
 * The cartridge proves the 0x28-byte layout, the twenty-entry enter table, the
 * construction order and the three callback roles. The class and member names
 * are inferred from the dMg* minigame family; see include/dMgState_c.h.
 */

#include "dMgState_c.h"

extern int ApproachLinear(s32 &value, s32 target, s32 step);

/* The per-state enter handlers, in state order. These are pointer-to-member
 * constants that mwccarm laid down in ov004's .data; the states they belong to
 * are not yet named, so they are still carried by address. */
extern dMgState_c::Callback data_ov004_020bc974;
extern dMgState_c::Callback data_ov004_020bc96c;
extern dMgState_c::Callback data_ov004_020bc964;
extern dMgState_c::Callback data_ov004_020bc95c;
extern dMgState_c::Callback data_ov004_020bc954;
extern dMgState_c::Callback data_ov004_020bc94c;
extern dMgState_c::Callback data_ov004_020bc944;
extern dMgState_c::Callback data_ov004_020bc92c;
extern dMgState_c::Callback data_ov004_020bc934;
extern dMgState_c::Callback data_ov004_020bc97c;
extern dMgState_c::Callback data_ov004_020bc984;
extern dMgState_c::Callback data_ov004_020bc99c;
extern dMgState_c::Callback data_ov004_020bc9a4;
extern dMgState_c::Callback data_ov004_020bca3c;
extern dMgState_c::Callback data_ov004_020bc9bc;
extern dMgState_c::Callback data_ov004_020bc9d4;
extern dMgState_c::Callback data_ov004_020bc9dc;
extern dMgState_c::Callback data_ov004_020bc9ec;
extern dMgState_c::Callback data_ov004_020bc9f4;
extern dMgState_c::Callback data_ov004_020bca0c;

// @symbol _ZN10dMgState_cC1Ev
dMgState_c::dMgState_c()
    : mState(-1), mTimer(0), unk_020(0), unk_024(0)
{
}

// @symbol _ZN10dMgState_c8SetStateEi
void dMgState_c::SetState(s32 state)
{
    static Callback sEnterTable[20] = {
        data_ov004_020bc974,
        data_ov004_020bc96c,
        data_ov004_020bc964,
        data_ov004_020bc95c,
        data_ov004_020bc954,
        data_ov004_020bc94c,
        data_ov004_020bc944,
        data_ov004_020bc92c,
        data_ov004_020bc934,
        data_ov004_020bc97c,
        data_ov004_020bc984,
        data_ov004_020bc99c,
        data_ov004_020bc9a4,
        data_ov004_020bca3c,
        data_ov004_020bc9bc,
        data_ov004_020bc9d4,
        data_ov004_020bc9dc,
        data_ov004_020bc9ec,
        data_ov004_020bc9f4,
        data_ov004_020bca0c,
    };

    mState = state;
    mEnter = sEnterTable[mState];
    mRender = 0;
    if (mEnter)
        (this->*mEnter)();
}

// @symbol _ZN10dMgState_c8BehaviorEv
void dMgState_c::Behavior()
{
    if (mState == -1)
        return;
    ApproachLinear(mTimer, 0, 1);
    if (mBehavior == 0)
        return;
    (this->*mBehavior)();
}

// @symbol _ZN10dMgState_c6RenderEv
void dMgState_c::Render()
{
    if (mState == -1)
        return;
    if (mRender == 0)
        return;
    (this->*mRender)();
}
