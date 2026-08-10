/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Enemy: 10 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ENEMY_H
#define ENEMY_H
#include "types.h"
#include "Actor.h"

/* fwd */
struct CylinderClsn;
struct ModelAnim;
struct Player;
struct Vector3_16;
struct WithMeshClsn;
struct a1_;
struct a2_;
struct clsn_;
struct flags;
struct mm_;
struct outAngle_;
struct ww_;
/* The actor heap and its deallocator, for the inline operator delete below.
   Spelt exactly as include/decl_common.h spells it -- see the note in Actor.h. */
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern "C" void *data_020a0eac;

struct Enemy : Actor {
    /* DERIVES FROM Actor, at last -- this header used to restate Actor's whole
       layout, and its own note said `Enemy : Actor` was "the real fix and is
       its own slice". Everything below 0x0d0 is Actor's and is inherited now;
       the thirteen fields that were duplicated here resolve through Actor.h,
       two of which (0x0a4, 0x0ac) were padding there until this class proved
       they are real.

       Actor is 0xd0, so Enemy's own fields start there and its 0x110 closes
       exactly on the subclasses that follow it. */
    /* FIVE FIELDS BELOW WERE PADDING HERE and are named on the same kind of
       evidence that took Actor's 0x0a4/0x0ac: the subclasses declare them and
       read them, and `X : Enemy` cannot compile unless they exist. The counts
       are how many of Enemy's 51 subclasses declare each -- 0x100 is in
       twenty-eight of them, which is not a coincidence. The names stay unk_
       where the meaning is unevidenced. */
    s32 mEatingPlayer;            /* 0x0d0 -- 4 subclasses */
    s32 mFloorNormalX;            /* 0x0d4 */
    s32 mFloorNormalY;            /* 0x0d8 */
    s32 mFloorNormalZ;            /* 0x0dc */
    s32 mWallNormalX;             /* 0x0e0 */
    s32 mWallNormalY;             /* 0x0e4 */
    s32 mWallNormalZ;             /* 0x0e8 */
    s16 mSpinRateX;               /* 0x0ec */
    s16 mSpinRateY;               /* 0x0ee */
    s16 mSpinRateZ;               /* 0x0f0 */
    u8  pad_0f2[0xe];
    s16 unk_100;                  /* 0x100 -- 28 subclasses */
    u16 mDeathTimer;              /* 0x102 */
    u16 unk_104;                  /* 0x104 -- 5 subclasses */
    u8 unk_106;                   /* 0x106 */
    u8 unk_107;                   /* 0x107 */
    u8  unk_108;                  /* 0x108 -- 10 subclasses */
    u8  pad_109[0x1];
    u8  unk_10a;                  /* 0x10a -- 5 subclasses */
    u8  pad_10b[0x1];
    s32 mDeathState;              /* 0x10c */

    virtual ~Enemy();                   /* slots 0 (D1), 1 (D0) */

    /* Enemy's own copy of Actor's inline operator delete, and it MUST STAY even
       though Enemy now derives from Actor. mwcc inlines the operator only when it
       finds it in the class itself or its IMMEDIATE base, and for a subclass of
       Enemy the immediate base is Enemy -- Actor is the grandparent, so Actor's
       copy is out of reach. Deleting this costs every Enemy subclass its D0.

       An earlier revision of this comment said the opposite: that Enemy was "still
       a flattened struct" and this copy should go once Enemy gained its real base.
       Enemy gained it, and following that instruction would have broken the D0
       route for 51 subclasses. See the long comment in Actor.h for why an inline
       member is what the ROM shows. */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }

    /* --- non-virtual --- */
    int AngleAwayFromWallOrCliff(WithMeshClsn & clsn_, short & outAngle_);
    int UpdateDeath(WithMeshClsn & clsn_);
    void UpdateWMClsn(WithMeshClsn & clsn_, unsigned int sel);
    /* Already a real method -- its own file builds _ZN5Enemy9SpawnCoinEv from a
       local `struct Enemy : Actor` shadow. Declared here so callers need not
       spell the mangled name. */
    void SpawnCoin();
    int SpawnParticlesIfHitOtherObj(CylinderClsn & clsn_);
    int UpdateKillByInvincibleChar(WithMeshClsn & ww_, ModelAnim & mm_, unsigned int flags);
    /* PROVISIONAL SIGNATURE -- do not migrate a caller against it yet. Two
       separate problems, both caller-side:

       (1) This name is currently attached to ov004 0x020ada40, which cannot be
           this method (it reads only r0 and range-checks it as a scalar). The
           code that IS this method is ov002 0x020ada40, still carrying the
           placeholder name func_ov002_020ada40. See notes/overlay-residency.md,
           which independently resolves this address to ov002.

       (2) The arity below is refuted by three call sites, which materialise a
           FOURTH argument in r3 with three different Fix12 constants --
           ov062 0x02117cf0 (0x46000), ov084 0x02129f94 (0x41000) and
           ov084 0x02129fcc (0x96000) -- with no other consumer before the bl.
           The ov002 callee ignores r3, so the trailing parameter is accepted
           and dropped. A two-reference prototype cannot emit those moves. */
    void KillByInvincibleChar(const Vector3_16 & a1_, Player & a2_);
};

/* Makes Enemy's size a claim the compiler enforces, and makes every header
   that derives from Enemy checkable by tools/check_header_offsets.py -- which
   otherwise skips a derived class whose base asserts no size. */
typedef char Enemy_size_must_be_0x110[sizeof(Enemy) == 0x110 ? 1 : -1];

#endif
