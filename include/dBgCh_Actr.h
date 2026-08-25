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
 *   slot 1  0x020373b8  ~dBgCh_Actr (D0)  - currently func_020373b8
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
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- the
 * key-function arrangement from include/ModelBase.h.
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
 *     name spells P10Vector3_16S5_). 0x12c is mSphereClsn.unk_10c; 0x130 is
 *     our own field, since sizeof(dBgCh_SphCrr) is pinned at 0x110. The FIELDS
 *     stay s32: whether they are pointer-typed is a separate question from
 *     what Init stores, and UpdateDiscreteNoLava passes them straight on to
 *     func_02038324, which takes ints.
 *   - The 0x1bc size is convergent evidence, not a single instruction: some
 *     forty embedding actor headers place their next field at exactly
 *     mWithMeshClsn + 0x1bc. Nothing in matched code allocates by sizeof.
 */

#ifdef __cplusplus

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
    u32 mFlags;                 /* 0x010 - word, see above */
    dActor_c *mActor;              /* 0x014 - Init arg 1 */
    Fix12i unk_018;             /* 0x018 - Init arg 2, the sphere radius */
    Fix12i unk_01c;             /* 0x01c - Init arg 3, a height */

    /* THE TWO SUB-OBJECTS, now typed as themselves. Both classes carry real,
       out-of-line constructors, which is what lets this class's constructor
       be written with an empty body: base step, vptr store and both member
       constructions are synthesised, in exactly the ROM's order. Interior
       offsets that used to be spelled on this flat list moved home with
       their bytes: 0x6c was mSphereClsn.disp (+0x4c), 0x90 is
       mSphereClsn.flags (+0x70), 0x128/0x12c are mSphereClsn.unk_108 /
       .unk_10c. 0x130 stays ours -- sizeof(dBgCh_SphCrr) is pinned at
       0x110, so 0x130 falls outside the sub-object (include/dBgCh_SphCrr.h). */
    dBgCh_SphCrr mSphereClsn;   /* 0x020 - spans [0x20,0x130) */
    s32 unk_130;                /* 0x130 - Init stores its Vector3_16 * arg 5 */
    dBgCh_Lin mRaycastLine;     /* 0x134 - spans [0x134,0x1b8) */
    Fix12i unk_1b8;             /* 0x1b8 - (?) Init sets 0x1000 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* DECLARED FIRST AND NEVER DEFINED AS A METHOD -- the key-function
       arrangement from include/ModelBase.h; nothing derives from this class,
       and D1/D0/C1 keep their own translation units. */
    virtual ~dBgCh_Actr();    /* slots 0 (D1), 1 (D0) */

    /* DECLARED, never defined as a method here -- src/_ZN10dBgCh_ActrC1Ev.cpp
       owns C1 (notes/ctor-migration.md section 2). Empty body: both member
       classes have real out-of-line ctors, so synthesis emits base step +
       vptr + both constructions in the ROM's order by itself. */
    dBgCh_Actr();

    /* --- non-virtual --- */
    void Init(dActor_c *actor, Fix12i radius, Fix12i height,
              Vector3_16 *a, Vector3_16 *b);  /* free definition, wall 6az */
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
};

typedef char dBgCh_Actr_size_must_be_0x1bc[
    sizeof(dBgCh_Actr) == 0x1bc ? 1 : -1];

#else

/* The C view is a compatibility shim for the .c translation units that have
   not been converted yet, and it is deliberately field-for-field IDENTICAL to
   what this header carried before the promotion -- including mFlags as a u8.
   Widening it here changed the code those files generate and turned
   UpdateContinuous from VERIFIED into 39 unresolvable relocations, i.e. from
   "proven equal to the ROM" into "cannot be checked at all". The corrected
   widths live on the C++ side above; a .c file inherits them when it is
   converted, not before. */
struct dBgCh_Actr {
    u8  pad_000[0x10];      /* dBgCh base */
    u32 mFlags;               /* 0x010 */
    struct dActor_c * mActor; /* 0x014 */
    s32 mRadius;            /* 0x018 */
    s32 mHeight;            /* 0x01c */
    u8  mSphereClsn;        /* 0x020 */
    u8  pad_021[0x4b];
    u8  mDisplacement;            /* 0x06c */
    u8  pad_06d[0x23];
    u8  mClsnFlags;         /* 0x090 */
    u8  pad_091[0x97];
    s32 mClsnScale;            /* 0x128 */
    s32 unk_12c;            /* 0x12c */
    s32 unk_130;            /* 0x130 */
    u8  mRaycastLine;       /* 0x134 */
    u8  pad_135[0x83];
    s32 mScale;            /* 0x1b8 */
};

/* In C the tag alone is not a type name, so an owner header that embeds a
   dBgCh_Actr BY VALUE -- which the cartridge's own destructors prove several do,
   see tools/dtor_members.py -- cannot spell the member without this. The
   definition and the typedef have to travel together: with the definition and no
   typedef the embed gets `undefined identifier', and then the owner's size assert
   gets `illegal constant expression' on top of it. */
typedef struct dBgCh_Actr dBgCh_Actr;

/* The C view substitutes for the C++ class only while it is the SAME SIZE. Once
   an owner embeds one by value the two branches lay that owner out differently if
   they ever disagree, and nothing else in the build compares them. Individual
   widths differ between the two on purpose (see the note above the #else); the
   total may not. */
typedef char dBgCh_Actr_size_must_be_0x1bc[
    sizeof(struct dBgCh_Actr) == 0x1bc ? 1 : -1];

#endif /* __cplusplus */

#endif /* DBGCH_ACTR_H */
