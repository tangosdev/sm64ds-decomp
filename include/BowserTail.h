/* Seeded from matched-function evidence by tools/gen_header.py, then given its
 * real base and real member type by hand.
 *
 * class BowserTail: 4 matched functions.
 *
 * One sub-object, and its offset is checked twice -- once by MovingCylinderClsn's
 * own size assertion, once by closing exactly on the next named field:
 *
 *     Actor               0x000 + 0x0d0 = 0x0d0   -> pad_0d0
 *     MovingCylinderClsn  0x0d4 + 0x034 = 0x108   -> unk_108
 *
 * sizeof is 0x118, which is not inferred from the fields: BowserTail_Spawn asks
 * ActorBase::operator new for 280 bytes.
 *
 * The position fields the generated header declared at 0x5c..0x64 are gone from
 * this half on purpose -- they are Actor's mPosX/mPosY/mPosZ and are inherited
 * now. The C half below still spells them, because a C translation unit has no
 * base class to inherit them from.
 *
 * Field NAMES for the unk_ entries are placeholders. */
#ifndef BOWSERTAIL_H
#define BOWSERTAIL_H
#include "types.h"
#include "MovingCylinderClsn.h"

#ifdef __cplusplus

#include "Actor.h"

struct BowserTail : Actor {
    u8  pad_0d0[0x4];
    /* Named by the class's own destructor calling MovingCylinderClsn's D1 at
       +0x0d4 -- a relocation the ROM build checks. */
    MovingCylinderClsn mMovingCylinderClsn;     /* 0x0d4 */
    u32 unk_108;                                /* 0x108 */
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
    /* MovingCylinderClsn member, named by the class's own destructor calling
       MovingCylinderClsn's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN10BowserTailD0Ev.c] */
    MovingCylinderClsn mMovingCylinderClsn;            /* 0x0d4 */
    u32 unk_108;            /* 0x108 */
    u8  pad_10c[0xc];
};

#endif /* __cplusplus */

#endif
