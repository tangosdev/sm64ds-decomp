#ifndef DBGCH_ACTR_H
#define DBGCH_ACTR_H

#include "types.h"
#include "dBgCh.h"

/* Only a pointer to it is stored, so a forward declaration is enough -- and
   this header reaches 826 translation units. */
struct dActor_c;

/* An dActor_c's mesh-collision query, vtable at 0x02099204 (still unnamed in
 * symbols.txt).
 *
 * VTABLE, 2 slots, read out of the ROM:
 *
 *   slot 0  0x020373f8  ~dBgCh_Actr (D1)
 *   slot 1  0x020373b8  ~dBgCh_Actr (D0)
 *
 * Exactly two: the word after slot 1 is the "5dBgPi" RTTI name string, not a
 * third slot. C1, D1 and the D0 all store the same literal, 0x02099204, into
 * [this+0], and no other word in the image references that address.
 *
 * DERIVES FROM dBgCh. Its typeinfo at 0x020991e0 is the 12-byte
 * __si_class_type_info kind whose base pointer is 0x020991c8 -- dBgCh's root
 * record. The ROM names this class dBgCh_Actr and the base dBgCh. C1 agrees
 * from the other side: it calls dBgCh's constructor (0x02035514) before
 * storing this vtable, and D1/D0 call dBgCh's D2 (0x020354d0) after tearing
 * the members down.
 *
 * NOTHING DERIVES FROM THIS. An image-wide scan finds the only reference to
 * typeinfo 0x020991e0 at 0x02099200, its own vtable header -- which is why
 * no separate D2 was ever emitted.
 *
 * D1 and D0 are both enrolled from real empty-body C++ definitions. mwcc owns
 * the reverse member order, the dBgCh base step, and the deleting variant's
 * collision-heap deallocation.
 *
 * THE TWO MEMBERS ARE TYPED SUB-OBJECTS (2026-08-24; they were flat blobs for
 * a long time because a dozen already-matched bodies reached into their
 * interiors by absolute offset -- self+0x30, +0x34, +0x90, +0x144, +0x148 --
 * while both sub-headers were auto-generated placeholders whose sizeof could
 * not be trusted). Both headers are now promoted and size-pinned: 0x30 is
 * mSphereClsn's query-sphere pos, 0x34 its radius, 0x6c mSphereClsn.disp,
 * 0x90 mSphereClsn.flags, 0x144 mRaycastLine's lineEnd.x, 0x148 .y.
 *
 * mFlags IS A WORD, NOT A BYTE. The placeholder had it as u8 and that was
 * wrong: every accessor does a full-word load/store, e.g. SetGroundFlag is
 * `add r1,r0,#0x10; ldr r0,[r1]; orr r0,r0,#0x10; str r0,[r1]', and
 * ClearAllGroundFlags is `bic r0,r0,#0x70'. Init stores a word zero there.
 * Bits in use: 0x10 on-ground, 0x20 just-hit-ground, 0x40 left-ground,
 * 0x80 limited-movement, 0x1000 no-update-pos-Y, 0x2000 no-update-pos, plus
 * 0x200/0x400/0x800/0x4000 behind unnamed get/set/clear triples.
 *
 * INFERRED, not pinned:
 *   - 0x12c / 0x130 receive Init's two Vector3_16 * parameters (the mangled
 *     name spells P10Vector3_16S5_). 0x12c is mSphereClsn.mMotionAngPtr; 0x130 is
 *     our own field, since sizeof(dBgCh_SphCrr) is pinned at 0x110. Both are
 *     Vector3_16 *: Init stores its arguments uncast and the Update* bodies
 *     pass them on to dBgS_MoveBgCrrPos as they are.
 *   - The 0x1bc size is convergent evidence, not a single instruction: some
 *     forty embedding actor headers place their next field at exactly
 *     mWithMeshClsn + 0x1bc. Nothing in matched code allocates by sizeof.
 */

#include "dBgCh_SphCrr.h"
#include "dBgCh_Lin.h"

struct dActor_c;

/* Forward-declared, deliberately not defined. common.h defines Vector3_16
   unconditionally and does NOT set VECTOR3_16_DEFINED, so any header that
   defines it -- guarded or not -- collides in every TU that also pulls
   common.h. Only pointers to it are needed here, so an incomplete type is
   enough and the collision is avoided outright. */
struct Vector3_16;   /* pointers only; see the note above */

struct dBgCh_Actr : dBgCh {
    u32 mFlags;       /* 0x010 - word, see above */
    dActor_c *mActor; /* 0x014 - Init arg 1 */
    fx32 mRadius;     /* 0x018 - Init arg 2, the sphere radius */
    fx32 mHeight;     /* 0x01c - Init arg 3, a height */

    /* THE TWO SUB-OBJECTS, now typed as themselves. Both classes carry real,
       out-of-line constructors, which is what lets this class's constructor
       be written with an empty body: base step, vptr store and both member
       constructions are synthesised, in exactly the ROM's order. Interior
       offsets that used to be spelled on this flat list moved home with
       their bytes: 0x6c was mSphereClsn.disp (+0x4c), 0x90 is
       mSphereClsn.flags (+0x70), 0x128/0x12c are mSphereClsn.mMaxNormalY /
       .mMotionAngPtr. 0x130 stays ours -- sizeof(dBgCh_SphCrr) is pinned at
       0x110, so 0x130 falls outside the sub-object (include/dBgCh_SphCrr.h). */
    dBgCh_SphCrr mSphereClsn;   /* 0x020 - spans [0x20,0x130) */
    Vector3_16 *mAngPtr;        /* 0x130 - Init stores its arg 5; dBgW::BeforeClsn's `ang` */
    dBgCh_Lin mRaycastLine;     /* 0x134 - spans [0x134,0x1ac) */
    Vector3 mGroundNormal;      /* 0x1ac - func_02037318 copies mSphereClsn.mFloorNormal here
                                   on a grounded frame. That payload is the floor normal with
                                   the largest y (dBgW_Kc's sphere query hands SetFloorNormal
                                   `&sn` when sn.y beats it); func_020371fc snaps only when .y
                                   is under 0xf20, i.e. on a slope. */
    fx32 mMaxNormalY;           /* 0x1b8 - every Update* copies it into
                                   mSphereClsn.mMaxNormalY, the face-normal Y
                                   ceiling of the sphere query; Init sets
                                   0x1000, the sphere's own accept-every-face
                                   default. Named after that destination. */

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* Defined out of line in src/engine/collision/d_bg_ch_actr.cpp. Nothing
       derives from this class, so D1 and D0 are the only variants emitted. */
    virtual ~dBgCh_Actr();    /* slots 0 (D1), 1 (D0) */

    /* DECLARED, never defined as a method here -- the consolidated TU owns
       C1 (notes/ctor-migration.md section 2). Empty body: both member
       classes have real out-of-line ctors, so synthesis emits base step +
       vptr + both constructions in the ROM's order by itself. */
    dBgCh_Actr();

    /* --- non-virtual --- */
    void Init(dActor_c *actor, fx32 radius, fx32 height, Vector3_16 *a,
              Vector3_16 *b); /* free definition, wall 6az */
    void ClearAllGroundFlags();
    void ClearGroundFlag();
    void ClearJustHitGroundFlag();
    void ClearLimMovFlag();
    void SetGroundFlag();
    void SetLimMovFlag();
    /* These two hide dBgCh's same-named methods rather than overriding them --
       they are separate symbols in the ROM, and they fan the call out to both
       sub-objects instead of touching this object's own dBgCh base. */
    void StartDetectingWater();
    void StopDetectingWater();
    void UpdateContinuous();
    void UpdateContinuousNoLava();
    void UpdateExtraContinous();  /* sic -- the ROM name is missing a 'u' */
    void UpdateDiscreteNoLava();
    void UpdateDiscreteNoLava_2();
    void UpdateContinuousExceptSlot0(); /* 0x02036acc; coined */
    void CommitFloorHit(s32); /* floor-result commit (0x020371b0); coined */
    void Unk_0203589c(); /* mSphereClsn.ResetQuery() */

    /* Flag helpers and the result accessor. Coined names: the ROM carries none
       for non-virtual members. Parameter-less on purpose; a named parameter in
       a class-scope declaration would renumber @NNN in every consumer TU. */
    int TestFlag200();           /* mFlags & 0x200 */
    void SetFlag200();           /* mFlags |= 0x200 */
    void ClearFlag200();         /* mFlags &= ~0x200 */
    int TestFlag400();           /* mFlags & 0x400 */
    void SetFlag400();           /* mFlags |= 0x400 */
    void ClearFlag400();         /* mFlags &= ~0x400 */
    int IsFlag800Clear();        /* (mFlags & 0x800) == 0 */
    void SetFlag800();           /* mFlags |= 0x800 */
    void ClearFlag800();         /* mFlags &= ~0x800 */
    void SetNoUpdatePosYFlag();  /* mFlags |= 0x1000 */
    void SetWaterTrackFlag();    /* mFlags |= 0x4000 */
    int IsWaterTrackFlagClear(); /* (mFlags & 0x4000) == 0 */
    void SetLeftGroundFlag();    /* mFlags |= 0x40 */
    int TestLeftGroundFlag();    /* mFlags & 0x40 */
    void SetJustHitGroundFlag(); /* mFlags |= 0x20 */
    dBgPi *GetSphereResult();    /* mSphereClsn as its dBgPi base */
    void SetRadius(fx32);        /* mRadius = radius */
    void SetHeight(fx32);        /* mHeight = height */
    /* Both the raycast line and the sphere collider take the flag. */
    void StartIgnoringVanishSurfaces();
    void StopIgnoringVanishSurfaces();
    void StartIgnoringType14Walls();
    void StopIgnoringType14Walls();

    /* --- const accessors, 0x02035564..0x02035727. `const` is not a style
           choice here: these mangle _ZNK, and without it the compiler emits
           _ZN and names symbols the ROM does not contain.

       NONE OF THE FIRST FIVE MAY BE DECLARED bool, and the bytes say so
       outright. Each is `and r0, r0, #mask` straight into `bx lr` with no
       normalisation, so IsOnGround returns 0 or 0x10 and IsOnWall returns 0
       or 8 -- the mask itself, not a truth value. A bool return would have
       forced the `movne r0,#1` pair, which is exactly what the other two DO
       carry: ShouldUpdatePos and ShouldUpdatePosY are `ands` followed by
       `moveq r0,#1` / `movne r0,#0`, because they invert the flag. That is
       the whole difference between the two shapes, and it is visible.

       The first five read mFlags (word, 0x10) except GetResultFlag1 and
       IsOnWall, which read the result byte at 0x90 with `ldrb' --
       mSphereClsn.flags, the byte inside the dBgCh_SphCrr sub-object, not a
       field of our own. --- */
    s32 GetResultFlag1() const;    /* mClsnFlags & 0x01 -- collision exists */
    s32 IsOnWall() const;          /* mClsnFlags & 0x08 */
    s32 GetLimMovFlag() const;     /* mFlags & 0x80 -- limited movement */
    s32 IsOnGround() const;        /* mFlags & 0x10 */
    s32 JustHitGround() const;     /* mFlags & 0x20 */
    s32 ShouldUpdatePos() const;   /* !(mFlags & 0x2000) */
    s32 ShouldUpdatePosY() const;  /* !(mFlags & 0x1000) */
    /* Both forward mSphereClsn's own accessor, which returns the dBgPi hit
       record, so they return it typed too. The return type is not in the
       mangled name and a pointer return is the same `bx lr` either way. */
    dBgPi *GetFloorResult() const; /* func_02037938(&mSphereClsn) -- mClsnResult1 */
    dBgPi *GetWallResult() const;  /* func_020378dc(&mSphereClsn) -- mClsnResult2 */
    s32 TouchesWater() const;      /* SurfaceInfo flag 0x20 on mSphereClsn.surface */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dBgCh_Actr_size_must_be_0x1bc[
    sizeof(dBgCh_Actr) == 0x1bc ? 1 : -1];
#endif

/* The arm9 tail-call veneers over the Update* members, defined in
   src/engine/collision/d_bg_ch_sph_crr_query.cpp. Each is its own ROM
   function, and callers branch to it rather than to the member, so a call
   has to name the veneer or its relocation moves. */
extern "C" {
void dBgCh_Actr_UpdateContinuous_Veneer(dBgCh_Actr *);     /* UpdateContinuous */
void func_020383f0(dBgCh_Actr *);                          /* UpdateContinuousNoLava */
void func_020383e4(dBgCh_Actr *);                          /* UpdateExtraContinous */
void dBgCh_Actr_UpdateDiscreteNoLava_veneer(dBgCh_Actr *); /* UpdateDiscreteNoLava */
}

/* The ground-normal latch and the ground snap the player runs on its mesh
   collision each frame, defined in src/engine/collision/d_bg_ch_actr.cpp;
   their method names are not recovered. */
void func_02037318(dBgCh_Actr *);            /* latch mGroundNormal */
extern "C" void func_020371fc(dBgCh_Actr *); /* snap onto a dBgCh_Gnd surface */

#endif /* DBGCH_ACTR_H */
