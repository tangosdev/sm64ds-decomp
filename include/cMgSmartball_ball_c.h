/* class cMgSmartball_ball_c, real ROM name confirmed by tools/rtti_extract.py
 * (build/rtti.json). Own vtable ov006:0x0213ec98, RTTI ov006:0x0213ebec
 * (_ZTI19cMgSmartball_ball_c), _ZTS19cMgSmartball_ball_c ov006:0x0213edc0.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x12c, from _Znwj(0x12c) in func_ov006_02115b0c. Base ends at 0x34,
 * so this class adds 0xf8 bytes -- the densest of the eleven children.
 *
 * FIELD EVIDENCE. All fields below 0x34 belong to the base and are reached
 * through inherited members (unk_004, mCurrent0/1, unk_020/024/028/02c,
 * unk_030); this class's own four functions never touch the base's
 * contested 0x31/0x32 span, so no raw cast is needed anywhere here.
 *
 * Most of the 0x34+ fields are both declared AND zeroed by this class's own
 * RestoreInitial, which is exhaustive -- it is the strongest single source
 * of the layout below (every array length and every scalar width comes
 * from that function's own loop bounds and store widths, not a guess).
 * SaveSnapshot and Update corroborate roughly half of the same offsets.
 *
 * SEVERAL NAMES ARE BORROWED, NOT INVENTED. func_ov006_02112ad8.c and
 * func_ov006_021128fc.c -- two out-of-scope helpers this class's own
 * SaveSnapshot calls with `this` -- each reinterpret the pointer through
 * their own local Obj-style struct cast and name a number of these exact
 * offsets (hit/hitA/hitB/hitC, anyHit, specialHit, nearby, targetIndex,
 * soundTimer, soundPlayed, state3a, state3b). Every one of those offsets is
 * ALSO independently touched by this class's own RestoreInitial, so the
 * width and existence of each field is evidenced in-scope; only the
 * spelling is borrowed. Everything else that lacks that corroboration
 * keeps an unk_ name.
 *
 * ONE GAP IS A DELIBERATE PAD DESPITE THAT SAME EVIDENCE: offsets 0x44-0x4b
 * are hitX/hitZ in func_ov006_02112ad8.c's own naming, but neither of THIS
 * class's own four functions (SaveSnapshot/Update/RestoreInitial/ctor)
 * touches them at all -- so, per the wing_c precedent, this header leaves
 * them an explicit pad rather than exposing named fields this migration's
 * own evidence does not reach. UNMODELLED, NOT UNREAD.
 *
 * pad_0e7[0x11] (0xe7-0xf7) is a genuine gap: RestoreInitial's own
 * exhaustive zero pass skips straight over it (nearby[] ends at 0xe6,
 * targetIndex starts at 0xf8), and func_ov006_02112ad8.c's Obj cast also
 * treats it as padding. Two independent callers agreeing it's unused is
 * as close to "confirmed empty" as this migration gets.
 *
 * pad_101/pad_111/pad_122/pad_12a are pure alignment gaps between adjacent
 * int fields (house style: explicit pads over implicit compiler-inserted
 * ones).
 *
 * CONSTRUCTED BY func_ov006_02114548, left a free function per the recipe
 * (this tree has migrated zero constructors). It calls the base constructor
 * and writes only this vtable and the base's unk_028 = 0x8000; it touches
 * nothing at or past 0x34, so it adds no evidence to the field list. */
#ifndef CMGSMARTBALL_BALL_C_H
#define CMGSMARTBALL_BALL_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

struct cMgSmartball_ball_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */

    s32 unk_034;      /* 0x034 -- countdown, decremented while >0 by
                          SaveSnapshot, zeroed by RestoreInitial */
    u8  unk_038;      /* 0x038 -- with unk_039/state3a/state3b, a small
                          zone-entry state group SaveSnapshot latches when
                          unk_034 has drained inside a trigger box */
    u8  unk_039;      /* 0x039 */
    u8  state3a;      /* 0x03a -- name from func_ov006_02112ad8.c's own
                          Obj cast of this same class; gates most of that
                          function's per-frame collision work */
    u8  state3b;      /* 0x03b -- ditto; set once unk_03c maxes out */
    s32 unk_03c;      /* 0x03c -- zone-dwell counter, capped at 0x12c,
                          reset to 0 outside the zone */
    s32 unk_040;      /* 0x040 -- countdown; func_ov006_02111dcc refuses to
                          restart it while positive, and its expiry clears
                          the base's unk_030 flag */

    /* UNMODELLED, NOT UNREAD -- see the header comment. */
    u8  pad_044[0x8]; /* 0x044-0x04b */

    u8  hit[0x20];      /* 0x04c */
    u8  hitA[0x20];     /* 0x06c */
    u8  hitB[0x20];     /* 0x08c */
    u8  hitC[0x20];     /* 0x0ac */
    u8  anyHit;          /* 0x0cc */
    u8  specialHit;       /* 0x0cd -- SaveSnapshot bails out early while set */
    u8  nearby[0x19];    /* 0x0ce */

    /* Genuine gap -- see the header comment. */
    u8  pad_0e7[0x11]; /* 0x0e7-0x0f7 */

    s32 targetIndex;  /* 0x0f8 -- set to -1 by RestoreInitial */
    s32 unk_0fc;      /* 0x0fc -- countdown, decremented while >0 by
                          SaveSnapshot, zeroed by RestoreInitial */
    u8  unk_100;      /* 0x100 -- selects between func_ov006_021128fc and
                          func_ov006_02112ad8 in SaveSnapshot */
    u8  pad_101[0x3]; /* 0x101-0x103 -- alignment */
    s32 unk_104;      /* 0x104 -- zeroed here; func_ov006_021128fc (out of
                          scope) sets it from elapsed-frame lag */
    s32 unk_108;      /* 0x108 -- read-only in this class's own functions;
                          copied into mCurrent0 when unk_110 is set */
    s32 unk_10c;      /* 0x10c -- ditto, copied into mCurrent1 */
    u8  unk_110;      /* 0x110 -- flag that triggers the unk_108/unk_10c
                          position reset */
    u8  pad_111[0x3]; /* 0x111-0x113 -- alignment */
    s32 unk_114;      /* 0x114 -- with unk_118, a position pair read/written
                          as a V2 by SaveSnapshot/RestoreInitial */
    s32 unk_118;      /* 0x118 */
    s32 unk_11c;      /* 0x11c -- incremented while the position pair above
                          isn't refreshed; zeroed by RestoreInitial */
    u8  unk_120;      /* 0x120 -- write-only in this class's own functions */
    u8  unk_121;      /* 0x121 -- gates the SaveSnapshot/Update branches
                          RestoreInitial derives from the base's unk_02c and
                          the scene's entity count */
    u8  pad_122[0x2]; /* 0x122-0x123 -- alignment */
    s32 soundTimer;   /* 0x124 -- countdown, decremented while >0 by
                          SaveSnapshot, zeroed by RestoreInitial */
    u8  soundPlayed;  /* 0x128 -- zeroed by RestoreInitial */
    u8  unk_129;      /* 0x129 -- flag SaveSnapshot latches once and
                          RestoreInitial clears */
    u8  pad_12a[0x2]; /* 0x12a-0x12b -- alignment */
};

typedef char cMgSmartball_ball_c_size_must_be_0x12c[sizeof(cMgSmartball_ball_c) == 0x12c ? 1 : -1];

#endif
