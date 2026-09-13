#ifndef DASCOIN_C_H
#define DASCOIN_C_H

#include "types.h"
#include "dActor_c.h"
#include "dCcAc_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Secret Coin (SECRET_COIN 329) -- RTTI ov002:0x0210b000 names daSCoin_c;
 * the debug table at 0x020903bc names SECRET_COIN. overlay_actors.md lists
 * the same actor slot as INVISIBLE_SECRET(329). SILVER_STAR(179) is a
 * different class (daStar_c). ov002 also carries POWER_STAR(178) and
 * STAR_MARKER(180), which this TU spawns / matches against.
 *
 * SIZE 0x114, the literal daSCoin_c_classInit passes to operator new.
 * Factory constructs dCcAc_c at 0xd4; D1 tears it down before ~dActor_c.
 * dActor_c ends at 0xd0; pad_0d0 is the 4-byte gap before mdCcAc_c.
 * 0xd4 + 0x34 = 0x108, and 0x108..0x114 is this class's POD group:
 *
 *   0x108  mLeaderUniqueID -- uniqueID of the elected leader.
 *   0x10c  pad_10c -- never read or written in this TU.
 *   0x10d  unk_10d -- param1 & 0xf; 020f05f4 matches it against a
 *                    STAR_MARKER at +0x1d9 and ORs 0x40 into the
 *                    POWER_STAR spawn param.
 *   0x10e  mGroupId -- (param1 >> 8) & 0xf.
 *   0x10f  mGroupRole -- 0 unassigned, 1 leader, 2 follower.
 *   0x110  mCollectedCount -- leader's tally; 5 destroys the set.
 *   0x111  mClsnDisabled -- nonzero skips mdCcAc_c.Update().
 *   0x112  unk_112 -- "already paired" flag, read/written by 020f051c.
 *   0x113  mDeathTimer -- DecIfAbove0_Byte; 0 means not dying.
 *
 * Behavior's election: a coin whose mGroupRole is still 0 and whose
 * mGroupId nibble is 0 or 0xf claims role 1, records uniqueID, then
 * walks FindWithActorID(0x149) and writes role 2 plus that uniqueID
 * into every other SECRET_COIN. The fifth collection spawns POWER_STAR
 * at the matching STAR_MARKER.
 */

struct daSCoin_c : dActor_c {
    u8  pad_0d0[0x4];
    dCcAc_c mdCcAc_c;            /* 0x0d4 */
    s32 mLeaderUniqueID;         /* 0x108 */
    u8  pad_10c[0x1];
    u8  unk_10d;                 /* 0x10d */
    u8  mGroupId;                /* 0x10e */
    u8  mGroupRole;              /* 0x10f -- 0 unassigned, 1 leader, 2 follower */
    u8  mCollectedCount;         /* 0x110 */
    u8  mClsnDisabled;           /* 0x111 */
    u8  unk_112;                 /* 0x112 */
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

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daSCoin_c_size_must_be_0x114[sizeof(daSCoin_c) == 0x114 ? 1 : -1];

#endif
