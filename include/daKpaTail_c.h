/* Seeded from matched-function evidence by tools/gen_header.py, then given its
 * real base and real member type by hand.
 *
 * class daKpaTail_c: Bowser's tail (actor ID 278, spawned by daKpa_c::InitResources).
 * A Player who grabs it is held by it while Bowser's own hold handler runs (see
 * daKpa_c.cpp). Four of its functions are real methods; its state workers live in
 * daKpa_c.cpp as extern "C" helpers.
 *
 * Readability pass: the members are named from how those workers use them and
 * BowserTail_State (daKpa_c.cpp) names the three mState values. Leftover: bit
 * 0x400 of the tail's mFlags, which daKpa_c.cpp only tests, is not named.
 *
 * One sub-object, and its offset is checked twice -- once by dCcAc_c's
 * own size assertion, once by closing exactly on the next named field:
 *
 *     dActor_c               0x000 + 0x0d0 = 0x0d0   -> pad_0d0
 *     dCcAc_c  0x0d4 + 0x034 = 0x108   -> mBowserUniqueID
 *
 * The last 0x10 bytes (0x108..0x118) are Bowser's uniqueID, the Player the tail
 * is holding, a three-state dispatch index, and two halfword counters. Each is
 * named from how daKpaTail_c's own state workers (func_ov060_02115b84 and its
 * three callees, in daKpa_c.cpp) read and write it.
 *
 * sizeof is 0x118, which is not inferred from the fields: daKpaTail_c_classInit asks
 * fBase_c::operator new for 280 bytes.
 *
 * The position fields the generated header declared at 0x5c..0x64 are gone from
 * this half on purpose -- they are dActor_c's mPosX/mPosY/mPosZ and are inherited
 * now. The C half below still spells them, because a C translation unit has no
 * base class to inherit them from.
 */
#ifndef DAKPATAIL_C_H
#define DAKPATAIL_C_H
#include "types.h"
#include "dCcAc_c.h"

#ifdef __cplusplus

#include "dActor_c.h"

struct daKpaTail_c : dActor_c {
    u8  pad_0d0[0x4];
    /* Named by the class's own destructor calling dCcAc_c's D1 at
       +0x0d4 -- a relocation the ROM build checks. */
    dCcAc_c mdCcAc_c;     /* 0x0d4 */
    /* daKpa_c's fBase_c::uniqueID. Behavior resolves it with
       dActor_c::FindWithID and puts the tail 0x8c units from his position, in
       the direction of his previous facing angle plus 0x8000. */
    u32 mBowserUniqueID;                                /* 0x108 */
    /* The Player this tail has grabbed (set by the grab check, cleared on release
       or when the hold times out); null when nothing is held. */
    dActor_c *mHeldPlayer;                              /* 0x10c */
    /* Index into the three-entry state table: 0 free (watching for a grab),
       1 cooldown after a release, 2 holding a Player. */
    s32 mState;                                         /* 0x110 */
    /* Frames since the last state change; the cooldown state ends when it
       passes 0x1e (30 frames, half a second at 60 fps). */
    u16 mTimer;                                         /* 0x114 */
    /* Frames of hold left: set to 0x96 (150) while the held Player's
       mAngleYSpeed is non-zero; otherwise it counts down, and at 0 the Player
       is dropped. */
    u16 mHoldCountdown;                                 /* 0x116 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~daKpaTail_c();              /* slots 16 (D1), 17 (D0) */

    /* --- non-virtual --- */
    int CleanupResources();
    int Render();
    int Behavior();
    int InitResources();

    /* state-table dispatcher and handlers, in ROM order */
    void func_ov060_02115b84();
    void func_ov060_02115c1c();
    void func_ov060_02115d50();
    void func_ov060_02115d68();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char BowserTail_size_must_be_0x118[sizeof(daKpaTail_c) == 0x118 ? 1 : -1];
#endif

#else

struct daKpaTail_c {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x6c];
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x0d4 -- a relocation the ROM build checks. */
    dCcAc_c mdCcAc_c;            /* 0x0d4 */
    u32 mBowserUniqueID;            /* 0x108 */
    void *mHeldPlayer;              /* 0x10c */
    s32 mState;                     /* 0x110 */
    u16 mTimer;                     /* 0x114 */
    u16 mHoldCountdown;             /* 0x116 */
};

#endif /* __cplusplus */

#endif
