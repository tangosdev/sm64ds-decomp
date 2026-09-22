#ifndef DMGSTATE_C_H
#define DMGSTATE_C_H

#include "types.h"

/* Non-polymorphic state controller embedded in dScMgBase_c. The class spelling
 * is inferred from the minigame subsystem's dMg* naming; the 0x28-byte layout,
 * construction order, three CodeWarrior member-function callbacks, 20-state
 * table, and per-frame behavior/render roles are all ROM-proven. */
/* HOST ONLY, and it changes no ROM byte: mwccarm never sees this block.
 * port/CMakeLists.txt sets /vmg /vmm so a pointer to member is eight bytes
 * WIDE, which is the ROM's width. It also makes MSVC give that pointer an
 * eight-byte ALIGNMENT, which the ROM does not: mwccarm aligns it to four.
 * The three Callback fields below therefore sit at the right offsets inside
 * this object either way, but the object itself claims alignment 8, and every
 * class that embeds it -- dScMgBase_c and all thirty-two minigame scenes --
 * picks up four bytes of padding in front of it and four more at its own tail.
 * Capping the packing at four restores the ROM's alignment without touching
 * the width: sizeof stays 0x28 and the field order is unchanged.
 */
#ifdef SM64DS_PLATFORM_PC
#pragma pack(push, 4)
#endif
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
#ifdef SM64DS_PLATFORM_PC
#pragma pack(pop)
#endif

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dMgState_c_size_must_be_0x28[
    sizeof(dMgState_c) == 0x28 ? 1 : -1];
#endif

#endif
