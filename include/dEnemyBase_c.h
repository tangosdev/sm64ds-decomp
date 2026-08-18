/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class dEnemyBase_c: 10 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef DENEMYBASE_C_H
#define DENEMYBASE_C_H
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

struct dEnemyBase_c : Actor {
    /* DERIVES FROM Actor, at last -- this header used to restate Actor's whole
       layout, and its own note said `dEnemyBase_c : Actor` was "the real fix and is
       its own slice". Everything below 0x0d0 is Actor's and is inherited now;
       the thirteen fields that were duplicated here resolve through Actor.h,
       two of which (0x0a4, 0x0ac) were padding there until this class proved
       they are real.

       Actor is 0xd0, so dEnemyBase_c's own fields start there and its 0x110 closes
       exactly on the subclasses that follow it. */
    /* FIVE FIELDS BELOW WERE PADDING HERE and are named on the same kind of
       evidence that took Actor's 0x0a4/0x0ac: the subclasses declare them and
       read them, and `X : dEnemyBase_c` cannot compile unless they exist. The counts
       are how many of dEnemyBase_c's 51 subclasses declare each -- 0x100 is in
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

    /* DECLARED, NEVER DEFINED HERE. Without it the compiler synthesises a
       constructor and inlines it into every subclass that has one, turning a
       single `bl _ZN12dEnemyBase_cC2Ev` into the whole Actor/fBase_c vtable-store
       chain written out in place. The ROM calls it at 0x020aed98. Same
       reasoning as the note on Model(); see include/Model.h. */
    dEnemyBase_c();

    virtual ~dEnemyBase_c();                   /* slots 0 (D1), 1 (D0) */

    /* dEnemyBase_c's own copy of Actor's inline operator delete, and it MUST STAY even
       though dEnemyBase_c now derives from Actor. mwcc inlines the operator only when it
       finds it in the class itself or its IMMEDIATE base, and for a subclass of
       dEnemyBase_c the immediate base is dEnemyBase_c -- Actor is the grandparent, so Actor's
       copy is out of reach. Deleting this costs every dEnemyBase_c subclass its D0.

       An earlier revision of this comment said the opposite: that dEnemyBase_c was "still
       a flattened struct" and this copy should go once dEnemyBase_c gained its real base.
       dEnemyBase_c gained it, and following that instruction would have broken the D0
       route for 51 subclasses. See the long comment in Actor.h for why an inline
       member is what the ROM shows. */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }

    /* --- non-virtual --- */
    int AngleAwayFromWallOrCliff(WithMeshClsn & clsn_, short & outAngle_);
    int UpdateDeath(WithMeshClsn & clsn_);
    void UpdateWMClsn(WithMeshClsn & clsn_, unsigned int sel);
    /* Already a real method -- its own file builds _ZN12dEnemyBase_c9SpawnCoinEv from a
       local `struct dEnemyBase_c : Actor` shadow. Declared here so callers need not
       spell the mangled name. */
    void SpawnCoin();
    int SpawnParticlesIfHitOtherObj(CylinderClsn & clsn_);
    /* Its own file still builds this from a local `struct dEnemyBase_c { char pad[0x100]; }`
       shadow, but SpawnParticlesIfHitOtherObj -- which shares its translation unit,
       ov002 0x020ad838..0x020aedbc -- calls it, so the merged TU needs the real
       declaration. Non-virtual, so it cannot move a vtable slot or change which TU
       is dEnemyBase_c's key function; ~dEnemyBase_c is still the first virtual declared. The
       second parameter is the CylinderClsn the caller was handed, passed as raw
       bytes because that class has no header here. */
    void SpawnMegaCharParticles(Actor & a, char * p);
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

/* Makes dEnemyBase_c's size a claim the compiler enforces, and makes every header
   that derives from dEnemyBase_c checkable by tools/check_header_offsets.py -- which
   otherwise skips a derived class whose base asserts no size. */
typedef char dEnemyBase_c_size_must_be_0x110[sizeof(dEnemyBase_c) == 0x110 ? 1 : -1];

#endif
