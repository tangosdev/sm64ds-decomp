/* class dScMgSlot3_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgSlot3_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable sits at ov006:0x0213eaa8 -- that word
 * is the vptr value, and the RTTI pointer one word below it is
 * _ZTI12dScMgSlot3_c, which is what ties the table to this class rather
 * than to a neighbour.
 *
 * SIZE 0x5044, from the factory's own `_ZN7fBase_cnwEm(0x5044)`
 * (dScMgSlot3_c_classInit in src/minigames/d_s_mg_slots.cpp). The evidenced field span stops at 0x5043,
 * so the trailing byte is alignment, not unclaimed space.
 *
 * MASCOT at 0x4f38: Toad at the table, a typed Mascot member
 * (include/Mascot.h). Mascot's own constructor and destructor (ov006
 * 0x020c221c and 0x020c21e4) build and tear it down as part of this class's,
 * and this class's Load, Behavior and Render hand its address to
 * Mascot_Load, Mascot_Update and Mascot_Draw. Its size is bounded by what
 * follows it, and that bound is 0xac, not the 0xb8 the first draft of this
 * header claimed: a THREE-ELEMENT s32 array sits at 0x4fe4, indexed by
 * five of this class's methods (the legacy Render's local
 * `struct T4fe4 { char pad[0x4fe4]; int vals[3]; }` names it outright, and
 * InitResources, Slot3_StateSpin, Slot3_StateJudge and
 * Slot3_ReelDigits all index it as
 * `[i]` or `+ i*4`). It closes exactly on mReelPeriod.
 *
 * OWN TAIL, 0x4ff0..0x5043: seventeen fields carried over verbatim from
 * this header's previous auto-generated form, all real matched access out
 * of dScMgSlot3_c's own methods. Offsets and widths are unchanged by this
 * migration -- the base contributes exactly 0x4f38 bytes, so every one of
 * them lands on the byte it landed on before. The four fields the old
 * header declared BELOW 0x4f38 (0x0a8, 0x0ac, 0x0b4, 0x0bc) are dropped:
 * all four are dScMgBase_c's own and are already declared there.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Its definition in src/minigames/d_s_mg_slots.cpp emits both
 * destructor variants. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one. The constructor is
 * inline in that file, so the factory dScMgSlot3_c_classInit (historical
 * alias func_ov006_0210c120), a plain `new dScMgSlot3_c`, holds it.
 *
 * SM64DS RTTI names the implementation dScMgSlot3_c.
 */
#ifndef DSCMGSLOT3_C_H
#define DSCMGSLOT3_C_H
#include "dScMgSingle3DBase_c.h"
#include "Mascot.h"

struct dScMgSlot3_c : dScMgSingle3DBase_c {
    dScMgSlot3_c();
    virtual ~dScMgSlot3_c();

    /* This class's own overrides, read off the ROM's vtable: the slots where the
       table differs from dScMgSingle3DBase_c's. The ones recovered off this
       class's own matched files are spelled WITHOUT the `virtual` keyword, the
       way include/daObjMarioCap_c.h and include/daObjRc_Dorifu_c.h spell
       theirs; the ones carrying a slot-number marker came from the
       dScMgBase_c slot campaign and keep that campaign's spelling, which
       includes the keyword. Either way an override of a virtual an ancestor
       already declares is implicitly virtual, so each reuses an existing slot
       and adds no field, and the 0x5044 assert below still holds. The
       destructor above is declared first and out of line, so it stays this
       class's key function; the production TU owns the compiler-emitted
       _ZTV12dScMgSlot3_c. */
    s32 InitResources();                  /* slot  0 */
    virtual void StartRound(int arg);     /* slot 18 */
    virtual int SaveScreenForMenu();      /* slot 29 */
    virtual void RestoreScreenFromMenu(); /* slot 30 */
    virtual void InitMainScreenBg();      /* slot 32 */
    s32 Behavior();                       /* slot  6 */
    s32 Render();                         /* slot  9 */

    Mascot mMascot;          /* 0x4f38 -- Toad at the table, see file banner */
    s32 mReelPos[3];         /* 0x4fe4 -- Fix12 scroll of each reel; seeded to a
                               random symbol * 0x50000 (0x50 px a symbol) */
    s32 mReelPeriod;         /* 0x4ff0 -- strip length * 80 pixels, in Fix12 */
    s32 mReelWinPos[3];      /* 0x4ff4 -- read instead of mReelPos during the
                               state-6 win-line pass */
    s32 mState;              /* 0x5000 -- index into the pointer-to-member table
                               sSlot3StateTable that Behavior dispatches */
    s32 mReelDifficulty;     /* 0x5004 -- selects strips/directions; wins add 1 while <5 */
    s32 mSpinSoundHandle;    /* 0x5008 -- returned by Sound_PlayIfNotActive */
    s32 mReelDrawY;          /* 0x500c -- while positive the marker rows are drawn
                               at +0x10 and +0x60 instead of a single 0x60 */
    s32 mWinColumn;          /* 0x5010 -- column of the payout caption
                               (n * 0x50 + 0x20); negative means none */
    s32 mPayoutRemaining;    /* 0x5014 -- credits still to award in state 3 */
    u16 mLamp1Angle;         /* 0x5018 -- -0x200 a tick while mState is 1 */
    u16 mLamp2Angle;         /* 0x501a -- -0x400 a tick */
    u8 mReelStrip[3][5];     /* 0x501c -- the symbol on each stop of each reel;
                               Render walks it modulo mStripLength */
    u8 mReelSpinning[3];     /* 0x502b -- touch clears this; stop at next boundary */
    u8 mReelStopped[3];      /* 0x502e -- stopped and result symbols captured */
    u8 mResultSymbols[3][3]; /* 0x5031 -- the 3x3 window the reels stopped on */
    u8 mStripLength;         /* 0x503a -- number of stops per reel */
    u8 mWinSymbol;           /* 0x503b -- matched against mResultSymbols; seeded
                               from 0x503c at InitResources */
    u8 mNextWinSymbol;       /* 0x503c -- replaces mWinSymbol after marker scrolling */
    u8 mStoppedReelCount;    /* 0x503d -- also selects the reel speed-table row */
    u8 mStateTimer;          /* 0x503e -- countdown for the current state */
    u8 mFrameCounter;        /* 0x503f -- +1 a Behavior tick; the win chime fires
                               on (n & 0x3f) == 0x20 */
    u8 mReelReverse[3];      /* 0x5040 -- 1 subtracts scroll, 0 adds it */
    u8 pad_5043[0x1];        /* 0x5043 -- rounds up to the 0x5044 boundary */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgSlot3_c_size_must_be_0x5044[sizeof(dScMgSlot3_c) == 0x5044 ? 1 : -1];
#endif

#endif
