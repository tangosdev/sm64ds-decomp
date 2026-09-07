/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class dEnemyBase_c: 10 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef DENEMYBASE_C_H
#define DENEMYBASE_C_H
#include "types.h"
#include "dActor_c.h"
#include "math/Fix12.h"

/* fwd */
struct dCc_c;
struct ModelAnim;
struct Player;
struct Vector3_16;
struct dBgCh_Actr;
struct a1_;
struct a2_;
struct clsn_;
struct flags;
struct mm_;
struct outAngle_;
struct ww_;
/* The actor heap and its deallocator, for the inline operator delete below.
   Spelt exactly as include/decl_common.h spells it -- see the note in dActor_c.h. */
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern "C" void *data_020a0eac;

struct dEnemyBase_c : dActor_c {
    /* DERIVES FROM dActor_c, at last -- this header used to restate dActor_c's whole
       layout, and its own note said `dEnemyBase_c : dActor_c` was "the real fix and is
       its own slice". Everything below 0x0d0 is dActor_c's and is inherited now;
       the thirteen fields that were duplicated here resolve through dActor_c.h,
       two of which (0x0a4, 0x0ac) were padding there until this class proved
       they are real.

       dActor_c is 0xd0, so dEnemyBase_c's own fields start there and its 0x110 closes
       exactly on the subclasses that follow it. */
    /* FIVE FIELDS BELOW WERE PADDING HERE and are named on the same kind of
       evidence that took dActor_c's 0x0a4/0x0ac: the subclasses declare them and
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
    /* 0x100 -- 28 subclasses. Frames spent in the current mState: the state
       handlers zero it the instant they change mState, and some count it up
       while others count it down through DecIfAbove0_Short. Derived
       independently by two passes over disjoint subclass sets (Unagi,
       MrBlizzard, Shark, PiranhaPlant, FirePiranhaPlantBig, daKrb_c) and
       (Whomp) -- see notes/enemy-provenance.md and
       notes/enemy-leaf-provenance.md. Named mStateTimer, not mTimer, because
       Klepto already owns an unrelated mTimer and the shorter name silently
       rebinds it. */
    s16 mStateTimer;              /* 0x100 */
    u16 mDeathTimer;              /* 0x102 */
    u16 unk_104;                  /* 0x104 -- 5 subclasses */
    u8 unk_106;                   /* 0x106 */
    /* 0x107 -- set while Yoshi has this enemy in his mouth. Both matched
       OnYoshiTryEat overrides refuse the bite while it is non-zero (BobOmb
       returns mEatenByYoshi == 0; Rabbit returns 0 when it is set), Rabbit's
       own mEatenTimer counts up only while it reads 1, and
       dEnemyBase_c::SpawnParticlesIfHitOtherObj changes how a cylinder
       collision is reported while it is set. Eight subclass Behaviors clear it
       on release. */
    u8 mEatenByYoshi;             /* 0x107 */
    u8  unk_108;                  /* 0x108 -- 10 subclasses */
    u8  pad_109[0x1];
    u8  unk_10a;                  /* 0x10a -- 5 subclasses */
    u8  pad_10b[0x1];
    s32 mDeathState;              /* 0x10c */

    /* DECLARED, NEVER DEFINED HERE. Without it the compiler synthesises a
       constructor and inlines it into every subclass that has one, turning a
       single `bl _ZN12dEnemyBase_cC2Ev` into the whole dActor_c/fBase_c vtable-store
       chain written out in place. The ROM calls it at 0x020aed98. Same
       reasoning as the note on Model(); see include/Model.h. */
    dEnemyBase_c();

    virtual ~dEnemyBase_c();                   /* slots 0 (D1), 1 (D0) */

    /* dEnemyBase_c's own copy of dActor_c's inline operator delete, and it MUST STAY even
       though dEnemyBase_c now derives from dActor_c. mwcc inlines the operator only when it
       finds it in the class itself or its IMMEDIATE base, and for a subclass of
       dEnemyBase_c the immediate base is dEnemyBase_c -- dActor_c is the grandparent, so dActor_c's
       copy is out of reach. Deleting this costs every dEnemyBase_c subclass its D0.

       An earlier revision of this comment said the opposite: that dEnemyBase_c was "still
       a flattened struct" and this copy should go once dEnemyBase_c gained its real base.
       dEnemyBase_c gained it, and following that instruction would have broken the D0
       route for 51 subclasses. See the long comment in dActor_c.h for why an inline
       member is what the ROM shows. */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }

    /* --- non-virtual --- */
    int AngleAwayFromWallOrCliff(dBgCh_Actr & clsn_, short & outAngle_);
    int UpdateDeath(dBgCh_Actr & clsn_);
    int UpdateYoshiEat(dBgCh_Actr & clsn_);
    void UpdateWMClsn(dBgCh_Actr & clsn_, unsigned int sel);
    /* Already a real method -- its own file builds _ZN12dEnemyBase_c9SpawnCoinEv from a
       local `struct dEnemyBase_c : dActor_c` shadow. Declared here so callers need not
       spell the mangled name. */
    /* Its parameters are SCALARS, and the mangled name says so. The old name
       spelled the first and last `Fix12<int>`, which the image refutes: mwcc
       homes a class-typed by-value parameter to the stack the moment a body
       reads it, and this function's ROM bytes read both straight from r1 and
       r3. Nothing anchors the class spelling either -- the whole image contains
       zero `_Z...` strings and zero occurrences of `5Fix12IiE`, so every
       mangled name in this tree is reconstructed and this one was a guess that
       the bytes disagree with. `Fix12i` keeps the fixed-point intent and
       mangles as plain `i`. See notes/mwccarm-codegen.md 6az. */
    int IsGoingOffCliff(dBgCh_Actr & clsn_, Fix12i down_, s16 slopeAng_,
                        bool detectWater_, bool skipPipeCheck_, Fix12i up_);
    void SpawnCoin();
    int SpawnParticlesIfHitOtherObj(dCc_c & clsn_);
    /* Its own file still builds this from a local `struct dEnemyBase_c { char pad[0x100]; }`
       shadow, but SpawnParticlesIfHitOtherObj -- which shares its translation unit,
       ov002 0x020ad838..0x020aedbc -- calls it, so the merged TU needs the real
       declaration. Non-virtual, so it cannot move a vtable slot or change which TU
       is dEnemyBase_c's key function; ~dEnemyBase_c is still the first virtual declared. The
       second parameter is the dCc_c the caller was handed, passed as raw
       bytes because that class has no header here. */
    void SpawnMegaCharParticles(dActor_c & a, char * p);
    int UpdateKillByInvincibleChar(dBgCh_Actr & ww_, ModelAnim & mm_, unsigned int flags);
    /* RESOLVED 2026-08-27. Both halves of the old PROVISIONAL note held up, and
       both are now fixed in config rather than carried as a caveat here.

       (1) WRONG ADDRESS. The name sat on ov004 0x020ada40, which reads only r0
           and range-checks it as a scalar -- it cannot be this method. ov002 and
           ov004 are alternates in one overlay slot (both based at 0x020ad3e0),
           which is why one address could carry two unrelated functions, and why
           every actor overlay's relocs.txt records this call as the ambiguous
           `module:overlays(2,4)`. What disambiguates it is not the config but the
           code: the ov002 body at the same address consumes r0/r1/r2 exactly as
           (enemy, Vector3_16 &, Player &) -- Vec3_HorzAngle(player + 0x5c,
           &this->pos), then Player::IncMegaKillCount(player) -- while ov004's
           only unambiguous callers are inside ov004 itself. The name moved to
           ov002 0x020ada40; ov004's function is `func_ov004_020ada40` again.

       (2) ARITY. Every `bl` to this address in the whole image was disassembled:
           19 of them, and the split is itself the proof of (1). The four inside
           ov004 (0x020ad97c, 0x020ada10, 0x020adb70, 0x020adbac) leave r3 alone,
           which is what a one-scalar-argument call looks like. All 15 outside it
           materialise a fourth argument in r3 -- the old note listed three. Most
           pass a Fix12 constant (0x46000, 0x41000, 0x96000, 0x32000, 0);
           ov064 0x02116904 and ov102 0x0214cc8c load one from an object; ov081
           0x02123988
           passes the RETURN of the virtual at vtable offset 0x74, which is slot
           29 -- and this method's own body calls that same slot 29 and adds the
           result to pos.y before spawning particles. So the caller hands over a
           height the callee then recomputes and the parameter is simply unused,
           which is why no body reads r3. Two src/ files declared a three-argument
           form and still matched: at those sites the `mov r3, #0` doubles as the
           source of the following `strh`, so the argument setup is invisible.

           Its type is `Fix12<int>`, by the sibling: the ROM's
           `dBgActor_c::UpdateKillByMegaChar(s16, s16, s16, Fix12<int>)` is the
           background-actor form of the same operation and ends in the same
           parameter. The mangled name below is the compiler's own answer for the
           declaration, not a hand-mangle. */
    void KillByInvincibleChar(const Vector3_16 & vel_, Player & player_, Fix12<int> unused_);
};

/* Makes dEnemyBase_c's size a claim the compiler enforces, and makes every header
   that derives from dEnemyBase_c checkable by tools/check_header_offsets.py -- which
   otherwise skips a derived class whose base asserts no size. */
typedef char dEnemyBase_c_size_must_be_0x110[sizeof(dEnemyBase_c) == 0x110 ? 1 : -1];

#endif
