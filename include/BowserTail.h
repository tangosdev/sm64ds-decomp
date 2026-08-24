/* Seeded from matched-function evidence by tools/gen_header.py, then given its
 * real base and real member type by hand.
 *
 * class BowserTail: 4 matched functions.
 *
 * One sub-object, and its offset is checked twice -- once by dCcAc_c's
 * own size assertion, once by closing exactly on the next named field:
 *
 *     dActor_c               0x000 + 0x0d0 = 0x0d0   -> pad_0d0
 *     dCcAc_c  0x0d4 + 0x034 = 0x108   -> mBowserUniqueID
 *
 * sizeof is 0x118, which is not inferred from the fields: BowserTail_Spawn asks
 * fBase_c::operator new for 280 bytes.
 *
 * The position fields the generated header declared at 0x5c..0x64 are gone from
 * this half on purpose -- they are dActor_c's mPosX/mPosY/mPosZ and are inherited
 * now. The C half below still spells them, because a C translation unit has no
 * base class to inherit them from.
 *
 * Field NAMES for the unk_ entries are placeholders. */
#ifndef BOWSERTAIL_H
#define BOWSERTAIL_H
#include "types.h"
#include "dCcAc_c.h"

#ifdef __cplusplus

#include "dActor_c.h"

struct BowserTail : dActor_c {
    u8  pad_0d0[0x4];
    /* Named by the class's own destructor calling dCcAc_c's D1 at
       +0x0d4 -- a relocation the ROM build checks. */
    dCcAc_c mdCcAc_c;     /* 0x0d4 */
    /* Bowser's fBase_c::uniqueID. Behavior resolves it with
       dActor_c::FindWithID and parks the tail 0x8c units out from his position
       along his facing angle. [_ZN10BowserTail8BehaviorEv.cpp] */
    u32 mBowserUniqueID;                                /* 0x108 */
    u8  pad_10c[0xc];

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~BowserTail();              /* slots 16 (D1), 17 (D0) */

    /* --- non-virtual --- */
    int CleanupResources();
    int Render();
    int Behavior();
    int InitResources();
};

typedef char BowserTail_size_must_be_0x118[sizeof(BowserTail) == 0x118 ? 1 : -1];

#else

struct BowserTail {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x6c];
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10BowserTailD0Ev.c] */
    dCcAc_c mdCcAc_c;            /* 0x0d4 */
    u32 mBowserUniqueID;            /* 0x108 */
    u8  pad_10c[0xc];
};

#endif /* __cplusplus */

#endif
