#ifndef DADKK_C_H
#define DADKK_C_H

#include "types.h"

/* RTTI names the class directly: _ZTS7daDkk_c / _ZTI7daDkk_c
 * (config/arm9/overlays/ov025/symbols.txt 0x021137e0/0x021137ec). The
 * factory the tree calls daDkk_c_classInit gives the real size:
 * fBase_c::operator new(928) == 0x3a0.
 *
 * BASE IS daDsnBase_c, NOT dBgActor_c DIRECTLY. daDkk_c_classInit stores
 * _ZTV11daDsnBase_c as a mid-construction vtable before this class's own --
 * daDsnBase_c.h documents the whole family (it and daDsn_c / Thwomp share
 * this base). daDsnBase_c's own destructor is defined inline in its class
 * body specifically so descendants inline its teardown rather than emitting
 * a `bl` to it; this class defines its own destructor inline for the
 * separate reason spelled out at the declaration below. D1 (0x021118c8) and D0
 * (0x02111928) both walk: own vptr, then daDsnBase_c's (inlined --
 * dExtShadowModel_c@0x338 then dExtAnmTexPat_c@0x324), then dBgActor_c's (also
 * inlined, per its own header's convention -- dBgW_KcMbg@0x124 and
 * dExtSimpleModel_c@0xd4), then dActor_c::~dActor_c.
 *
 * daDsnBase_c ends its own data at 0x360; this class's own storage starts
 * there. daDsnBase_c.h notes 0x360..0x3a0 exists in both leaves
 * (daDkk_c_classInit allocates 0x3a0, Thwomp_Spawn 0x3a4) but stays declared
 * per-leaf until a later pass shows both reading the same offsets. The
 * daDsnBase_c methods this class runs (its states 0..4 and the per-frame
 * shadow refresh) read and write the whole span through d_a_dsn_base.cpp's
 * DsnCycle view, which is the evidence for most of the names below:
 *
 *   0x360  mShadowMtx -- UpdateDropShadow copies the model matrix here,
 *          lowers its translation row, and hands it to the drop shadow.
 *   0x390  mRiseY -- the top of the rise. daDsnBase_c::Init seeds it at
 *          mPosY + 0x190000; this class's InitResources then raycasts up
 *          and, on a hit, lowers it to 0x190000 under the ceiling. State 0
 *          (StateRise) climbs to it. Was mProbeHeight.
 *   0x394  mGroundY -- Init's ground probe writes it; state 5 here and the
 *          base's fall both snap mPosY to it.
 *   0x398  mState, the selector Behavior switches on (cases 0..7).
 *   0x39c  mTurnTargetAngle -- state 6 writes mAngleY + 0x8000, state 7
 *          steps mAngleY toward it.
 *   0x39e  mTimer -- the cycle's frame countdown: the base's hover and rest
 *          states spend it, and here it is the post-landing pause.
 *   0x39f  mLandCount -- Init zeroes it, state 5 counts landings, state 6
 *          turns the actor on the fourth, state 7 zeroes it again.
 *
 * Slot 29, OnAimedAtWithEgg, is an override this class supplies beyond the
 * two (InitResources, Behavior) daDsnBase_c leaves abstract; the signature
 * matches dActor_c's own declaration (include/dActor_c.h, slot 29).
 *
 * SM64DS RTTI names the implementation daDkk_c. The reconstructed factory
 * daDkk_c_classInit (historical alias Grindel_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_DONKAKU
 * (historical alias Grindel_SpawnInfo) is its registry descriptor. Overlay
 * actor 162 is GRINDEL in symbols/overlay_actors.md and DONKAKU in the ROM
 * debug table -- this class, not daDgr_c (DONGURU, actor 163).
 */

#ifdef __cplusplus

#include "daDsnBase_c.h"

struct daDkk_c : daDsnBase_c {
    /* The drop shadow's matrix; daDsnBase_c's per-frame shadow refresh
       fills it. Kept as raw bytes: a Matrix4x3 member would add a
       destructor. Nothing in d_a_dkk.cpp touches it. */
    u8 mShadowMtx[0x30];  /* 0x360 */
    s32 mRiseY;           /* 0x390 -- top of the rise (state 0) */
    s32 mGroundY;         /* 0x394 -- ground height the falls snap to */
    s32 mState;           /* 0x398 */
    s16 mTurnTargetAngle; /* 0x39c -- target facing for the turn */
    u8 mTimer;            /* 0x39e -- cycle countdown; post-landing pause */
    u8 mLandCount;        /* 0x39f -- landings since the last turn */

    /* --- vtable --- */
    /* INLINE ON PURPOSE, and this is a byte-level requirement rather than a
       style choice. mwccarm 2004/b56's destructor-variant order is a closed,
       measured lever set: a destructor DEFINED IN THE CLASS BODY emits D1 then
       D0 and nothing else, while an out-of-line definition emits D2, D0, D1 and
       adds a homeless D2. The cartridge holds D1 at 0x021118c8 BELOW D0 at
       0x02111928 and carries no D2 at all, so only the in-class form reproduces
       it. The out-of-line spelling this header used to carry came from the
       one-function shards, each of which compiled alone and so never exposed
       the ordering.

       EMPTY, BUT NOT INERT. This class adds no member of its own with a
       destructor, yet the body is 0x60 bytes: it stores this class's vptr, then
       daDsnBase_c's -- inlined, because daDsnBase_c.h defines its destructor in
       its class body -- which destroys dExtShadowModel_c@0x338 then
       dExtAnmTexPat_c@0x324, then dBgActor_c's, also inlined, which destroys
       dBgW_KcMbg@0x124 and dExtSimpleModel_c@0xd4, before chaining to dActor_c::~dActor_c.

       Inlining it also moves this class's ABI key function down to
       InitResources, the first declared virtual that is now neither inline nor
       pure. That is a member src/game/actors/d_a_dkk.cpp defines, so that TU is the
       one that emits _ZTV7daDkk_c. */
    virtual ~daDkk_c() {}

    int InitResources();
    int Behavior();
    int OnAimedAtWithEgg();

    int StateTurn();
    void StateLandPause();
    void StateFall();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daDkk_c_size_must_be_0x3a0[sizeof(daDkk_c) == 0x3a0 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DADKK_C_H */
