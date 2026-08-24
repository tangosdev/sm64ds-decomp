/* class cMgSmartball_slot_c, real ROM name confirmed by tools/rtti_extract.py
 * (build/rtti.json). Own vtable ov006:0x0213ed74, RTTI ov006:0x0213ec70
 * (_ZTI19cMgSmartball_slot_c), _ZTS19cMgSmartball_slot_c ov006:0x0213edf0.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x88, from _Znwj(0x88) in func_ov006_02115b0c. Base ends at 0x34, so
 * this class adds 0x54 bytes. This class's own four functions never touch
 * the base's 0x31-0x33 region, so no raw cast is needed anywhere here
 * (unlike wing_c/ana_c/board_c).
 *
 * FIELD EVIDENCE. The constructor (func_ov006_021101bc) placement-constructs
 * two 3-element, 8-byte-stride arrays via func_020733a8(this+0x34,3,8,...)
 * and func_020733a8(this+0x4c,3,8,...) -- that call is the source of the
 * two arrays' bounds and 8-byte stride. RestoreInitial is exhaustive over
 * everything from there to the end of the class and is the strongest single
 * source for the rest of the layout: every loop bound and store width below
 * comes from its own zero pass, not a guess. Update corroborates the first
 * array's shape (it already used a `struct {int a,b;} arr[3]` cast at 0x34
 * in the pre-migration matched source -- kept verbatim as a nested member
 * type here). SaveSnapshot corroborates the four scalars from 0x73 to 0x84.
 * FIELD NAMES and their evidence are tabulated in
 * notes/smartball-provenance.md.
 *
 * mReelOffset/mReelVel ARE ARRAYS OF (x,y) PAIRS, not flat scalars, because
 * Update indexes them with a real stride-8 loop in source that already
 * byte-matched before this migration -- the nested struct type is carried over
 * unchanged, only the local cast became a real member. Update is also where
 * the pair members get their names: it adds `[i].x` to mCurrent0 and `[i].y`
 * to mCurrent1 to place each sprite.
 *
 * WHAT THE CLASS IS: a three-reel slot machine, and every name below comes
 * from reading its three out-of-scope helpers alongside its own four
 * functions.
 *   - func_ov006_0210fb04 starts a spin: raises mIsSpinning and
 *     mIsSpeedingUp, arms mSpinTimer with 0xb4 (180 frames), plays sound
 *     0x160 -- and, if a spin is already running, hands the credit back by
 *     incrementing mSpinsQueued instead.
 *   - func_ov006_0210fa6c begins the stop: raises mIsSlowingDown, clears every
 *     mReelStopping, and rolls each mReelStopCount to a random 3..5.
 *   - func_ov006_0210fb58 runs every frame. While mIsSpeedingUp it ramps each
 *     mReelVel[i].y up toward 0x3000 and drops the flag when all three have
 *     arrived; while mIsSlowingDown it ramps them back down. It always adds
 *     mReelVel[i].y into mReelOffset[i].y and wraps that at 0x30000 -- three
 *     symbols of 0x10000 each, which is what makes mReelOffset[i].y a reel
 *     position rather than a plain offset. Each time a decelerating reel
 *     crosses a symbol boundary it spends one mReelStopCount; at zero the reel
 *     snaps to the boundary, its speed is zeroed and sound 0x19e plays. When
 *     all three have stopped on the SAME symbol it pays out (three cases:
 *     0, 0x10000, 0x20000) and sets mSpinCooldown to 0x3c.
 *
 * pad_076[2] (0x76-0x77) is a genuine gap: RestoreInitial's own exhaustive
 * zero pass jumps straight from the three flag bytes at 0x73-0x75 to the
 * s32 countdown at 0x78, skipping it entirely. UNMODELLED, NOT UNREAD.
 *
 * CONSTRUCTED BY func_ov006_021101bc, left a free function per the recipe
 * (this tree has migrated zero constructors), with only its vtable-symbol
 * reference renamed. */
#ifndef CMGSMARTBALL_SLOT_C_H
#define CMGSMARTBALL_SLOT_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

/* NOTE FOR ANYONE EDITING THIS HEADER: tools/check_header_offsets.py CANNOT
 * parse the two anonymous nested-struct arrays below. It reports them as
 * "2 unparsed" and then computes the struct as spanning 0x58 rather than
 * 0x88 -- and still prints "0 mismatched", which reads like a pass. The
 * offsets in this header are therefore NOT machine-checked by that gate.
 * What actually holds them is the compile-time size assert at the bottom
 * plus the byte match itself, since a wrong offset changes the emitted
 * instruction. Do not trust a clean check_header_offsets run on this file. */
struct cMgSmartball_slot_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */

    /* 0x034 -- per-reel render offset from mCurrent, placement-constructed by
       the ctor. Update adds .x/.y to mCurrent0/mCurrent1; .y is the reel's
       scroll position, wrapped at 0x30000 by func_ov006_0210fb58. .x is
       zeroed by RestoreInitial and never written anywhere else. */
    struct { s32 x; s32 y; } mReelOffset[3];

    /* 0x04c -- per-reel scroll speed, placement-constructed by the ctor. Only
       .y is ever used: func_ov006_0210fb58 ramps it up to 0x3000 while
       mIsSpeedingUp, back down to 0x200/0x800 while mIsSlowingDown, and adds
       it into mReelOffset[i].y every frame. */
    struct { s32 x; s32 y; } mReelVel[3];

    s32 mReelStopCount[3]; /* 0x064 -- symbols this reel still has to travel
                               before it stops. func_ov006_0210fa6c rolls it to
                               a random 3..5 when the stop begins;
                               func_ov006_0210fb58 spends one per symbol
                               boundary crossed and snaps the reel at zero. */
    u8  mReelStopping[3]; /* 0x070 -- per-reel: this reel has slowed to its
                              final creep and may now stop on a boundary. Set
                              by func_ov006_0210fb58 when the speed bottoms out
                              at 0x800, cleared by func_ov006_0210fa6c. */
    u8  mIsSpinning;    /* 0x073 -- a spin is in progress. Raised by
                            func_ov006_0210fb04, cleared by
                            func_ov006_0210fb58 once every reel has settled.
                            SaveSnapshot will not start a spin while it is
                            set, and func_ov006_0210fb04 refunds the credit. */
    u8  mIsSpeedingUp;  /* 0x074 -- the spin-up phase: func_ov006_0210fb58
                            ramps every mReelVel[i].y toward 0x3000 and clears
                            this once all three have got there. */
    u8  mIsSlowingDown; /* 0x075 -- the spin-down phase, raised by
                            func_ov006_0210fa6c; while set,
                            func_ov006_0210fb58 brakes the reels and counts
                            mReelStopCount down. */

    /* Genuine gap -- see the header comment. */
    u8  pad_076[2]; /* 0x076-0x077 */

    s32 mSpinsQueued;  /* 0x078 -- spins still owed. SaveSnapshot spends one and
                           starts a spin when mIsSpinning is clear and
                           mSpinCooldown has drained; func_ov006_0210fb04 hands
                           it straight back if a spin was already running.
                           Zeroed by RestoreInitial. */
    s32 mSpinCooldown; /* 0x07c -- frames before the next spin may start, set to
                           0x3c by func_ov006_0210fb58 when a spin finishes and
                           aged by SaveSnapshot. Zeroed by RestoreInitial. */
    s32 mSpinTimer;    /* 0x080 -- how long the reels free-run: armed with 0xb4
                           by func_ov006_0210fb04, aged by SaveSnapshot, and on
                           reaching exactly 0 it calls func_ov006_0210fa6c to
                           begin the stop. Zeroed by RestoreInitial. */
    s32 mSoundHandle;  /* 0x084 -- the handle Sound_PlayIfNotActive returns for
                           the reel-spin loop, fed back in as its own first
                           argument by func_ov006_0210fb58 and cleared there
                           when the loop should stop. This class's own
                           SaveSnapshot/Update never touch it; RestoreInitial
                           zeroes it. */
};

typedef char cMgSmartball_slot_c_size_must_be_0x88[sizeof(cMgSmartball_slot_c) == 0x88 ? 1 : -1];

#endif
