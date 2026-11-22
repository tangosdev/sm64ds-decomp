#ifndef DASCOIN_C_H
#define DASCOIN_C_H

#include "types.h"
#include "dActor_c.h"
#include "dCcAc_c.h"

/* Secret Coin (SECRET_COIN 329). RTTI ov002:0x0210b000 names daSCoin_c; the
 * debug table names SECRET_COIN (overlay_actors spells it INVISIBLE_SECRET).
 * Coins sit in a level in groups of five; collecting the fifth spawns
 * POWER_STAR at the STARBASE marker whose mStarID matches the group's
 * mStarID.
 *
 * SIZE 0x114. dActor_c ends at 0xd4; the class's POD group is 0x108..0x114:
 *
 *   0x0d4  mdCcAc_c        -- touch collider
 *   0x108  mLeaderUniqueID -- uniqueID of the elected leader
 *   0x10d  mStarID         -- param1 & 0xf; matched against
 *                             daStarBase_c::mStarID, then OR'd 0x40 into the
 *                             POWER_STAR spawn param
 *   0x10e  mGroupId        -- (param1 >> 8) & 0xf
 *   0x10f  mGroupRole      -- 0 unassigned, 1 leader, 2 follower
 *   0x110  mCollectedCount -- leader's tally; 5 destroys the set
 *   0x111  mClsnDisabled   -- nonzero skips mdCcAc_c.Update()
 *   0x112  mBlockScanDone  -- LinkToBlock already ran (one-shot guard)
 *   0x113  mDeathTimer     -- DecIfAbove0_Byte; 0 means not dying
 */

struct daSCoin_c : dActor_c {
    dCcAc_c mdCcAc_c;            /* 0x0d4 */
    s32 mLeaderUniqueID;         /* 0x108 */
    u8  pad_10c[0x1];
    u8  mStarID;                 /* 0x10d */
    u8  mGroupId;                /* 0x10e */
    u8  mGroupRole;              /* 0x10f -- 0 unassigned, 1 leader, 2 follower */
    u8  mCollectedCount;         /* 0x110 */
    u8  mClsnDisabled;           /* 0x111 */
    u8  mBlockScanDone;          /* 0x112 */
    u8  mDeathTimer;             /* 0x113 */

    /* INLINE IS LOAD-BEARING. Out of line, mwccarm emits D0 before D1
       (cartridge is 0x020f03c4 D1 then 0x020f03f4 D0) plus a D2 with no
       ROM home. Empty body: mdCcAc_c teardown, the vptr store and
       dActor_c's teardown are synthesised. Key function is InitResources,
       the first declared non-inline virtual. */
    virtual ~daSCoin_c() {}          /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();    /* slot  0 */
    virtual s32  CleanupResources(); /* slot  3 */
    virtual s32  Behavior();         /* slot  6 */

    /* Readable inferred names, not ROM-authenticated original spellings.
     * Address evidence is recorded in symbols/actor_renames.tsv. */
    void Collect();      /* was func_ov002_020f0438 */
    void LinkToBlock();  /* was func_ov002_020f051c */
    void SpawnStar();    /* was func_ov002_020f05f4 */
};

typedef char daSCoin_c_size_must_be_0x114[sizeof(daSCoin_c) == 0x114 ? 1 : -1];

#endif
