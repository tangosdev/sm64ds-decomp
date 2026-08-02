#ifndef WITHMESHCLSN_H
#define WITHMESHCLSN_H

#include "types.h"
#include "BgCh.h"

/* An Actor's mesh-collision query, vtable at 0x02099204 (still unnamed in
 * symbols.txt).
 *
 * VTABLE, 2 slots, read out of the ROM:
 *
 *   slot 0  0x020373f8  ~WithMeshClsn (D1)
 *   slot 1  0x020373b8  ~WithMeshClsn (D0)  - currently func_020373b8
 *
 * Exactly two: the word after slot 1 is the "5dBgPi" RTTI name string, not a
 * third slot. C1, D1 and the D0 all store the same literal, 0x02099204, into
 * [this+0], and no other word in the image references that address.
 *
 * DERIVES FROM BgCh. Its typeinfo at 0x020991e0 is the 12-byte
 * __si_class_type_info kind whose base pointer is 0x020991c8 -- BgCh's root
 * record. The ROM names this class dBgCh_Actr and the base dBgCh. C1 agrees
 * from the other side: it calls BgCh's constructor (0x02035514) before
 * storing this vtable, and D1/D0 call BgCh's D2 (0x020354d0) after tearing
 * the members down.
 *
 * NOTHING DERIVES FROM THIS. An image-wide scan finds the only reference to
 * typeinfo 0x020991e0 at 0x02099200, its own vtable header -- which is why
 * no separate D2 was ever emitted.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- the
 * key-function arrangement from include/ModelBase.h.
 *
 * WHY THE FIELD LIST IS FLAT. Two members are whole sub-objects: a
 * SphereClsn at 0x20 (C1 does `add r0,r4,#0x20' then SphereClsn's C1) and a
 * RaycastLine at 0x134 (`add r0,r4,#0x134' then RaycastLine's C1). They are
 * NOT typed as such here, because a dozen already-matched bodies reach into
 * their interiors by absolute offset -- self+0x30, +0x34, +0x90, +0x144,
 * +0x148 -- and SphereClsn.h / RaycastLine.h are still auto-generated
 * placeholders whose sizeof cannot be trusted. Typing the members would
 * rewrite those bodies for no byte benefit. Interior offsets worth knowing:
 * 0x30 sphere.result, 0x34 its SurfaceInfo, 0x6c a Vector3 displacement,
 * 0x90 the result-flag byte, 0x144 line.result, 0x148 its SurfaceInfo.
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
 *     name spells P10Vector3_16S5_), but they are left as s32 here. Whether
 *     the FIELDS are pointer-typed is a separate question from what Init
 *     stores, and UpdateDiscreteNoLava passes them straight on to
 *     func_02038324, which takes ints. They could also formally be the
 *     SphereClsn's tail; no SphereClsn-side access to them exists, so they
 *     are treated as ours.
 *   - 0x1b8 likewise sits at the RaycastLine boundary.
 *   - The 0x1bc size is convergent evidence, not a single instruction: some
 *     forty embedding actor headers place their next field at exactly
 *     mWithMeshClsn + 0x1bc. Nothing in matched code allocates by sizeof.
 */

#ifdef __cplusplus

struct Actor;

/* Forward-declared, deliberately not defined. common.h defines Vector3_16
   unconditionally and does NOT set VECTOR3_16_DEFINED, so any header that
   defines it -- guarded or not -- collides in every TU that also pulls
   common.h. Only pointers to it are needed here, so an incomplete type is
   enough and the collision is avoided outright. */
struct Vector3_16;   /* pointers only; see the note above */

struct WithMeshClsn : BgCh {
    u32 mFlags;                 /* 0x010 - word, see above */
    Actor *mActor;              /* 0x014 - Init arg 1 */
    Fix12i unk_018;             /* 0x018 - Init arg 2, the sphere radius */
    Fix12i unk_01c;             /* 0x01c - Init arg 3, a height */
    u8  mSphereClsn;            /* 0x020 - SphereClsn sub-object starts here */
    u8  pad_021[0x4b];
    u8  unk_06c;                /* 0x06c - a Vector3 displacement */
    u8  pad_06d[0x23];
    u8  mClsnFlags;             /* 0x090 - result-flag byte, ldrb/strb */
    u8  pad_091[0x97];
    s32 unk_128;                /* 0x128 - copied from unk_1b8 each update */
    s32 unk_12c;                /* 0x12c - Init stores its Vector3_16 * arg 4 */
    s32 unk_130;                /* 0x130 - Init stores its Vector3_16 * arg 5 */
    u8  mRaycastLine;           /* 0x134 - RaycastLine sub-object starts here */
    u8  pad_135[0x83];
    Fix12i unk_1b8;             /* 0x1b8 - (?) Init sets 0x1000 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~WithMeshClsn();    /* slots 0 (D1), 1 (D0) */

    /* --- non-virtual --- */
    void Init(Actor *actor, Fix12i radius, Fix12i height,
              Vector3_16 *a, Vector3_16 *b);  /* free definition, wall 6az */
    void ClearAllGroundFlags();
    void ClearGroundFlag();
    void ClearJustHitGroundFlag();
    void ClearLimMovFlag();
    void SetGroundFlag();
    void SetLimMovFlag();
    /* These two hide BgCh's same-named methods rather than overriding them --
       they are separate symbols in the ROM, and they fan the call out to both
       sub-objects instead of touching this object's own BgCh base. */
    void StartDetectingWater();
    void StopDetectingWater();
    void UpdateDiscreteNoLava();
    void UpdateDiscreteNoLava_2();
};

typedef char WithMeshClsn_size_must_be_0x1bc[
    sizeof(WithMeshClsn) == 0x1bc ? 1 : -1];

#else

/* The C view is a compatibility shim for the .c translation units that have
   not been converted yet, and it is deliberately field-for-field IDENTICAL to
   what this header carried before the promotion -- including mFlags as a u8.
   Widening it here changed the code those files generate and turned
   UpdateContinuous from VERIFIED into 39 unresolvable relocations, i.e. from
   "proven equal to the ROM" into "cannot be checked at all". The corrected
   widths live on the C++ side above; a .c file inherits them when it is
   converted, not before. */
struct WithMeshClsn {
    u8  pad_000[0x10];      /* BgCh base */
    u8  mFlags;             /* 0x010 - u32 on the C++ side; see above */
    u8  pad_011[0x3];
    u8  mActor;             /* 0x014 */
    u8  pad_015[0x3];
    s32 unk_018;            /* 0x018 */
    s32 unk_01c;            /* 0x01c */
    u8  mSphereClsn;        /* 0x020 */
    u8  pad_021[0x4b];
    u8  unk_06c;            /* 0x06c */
    u8  pad_06d[0x23];
    u8  mClsnFlags;         /* 0x090 */
    u8  pad_091[0x97];
    s32 unk_128;            /* 0x128 */
    s32 unk_12c;            /* 0x12c */
    s32 unk_130;            /* 0x130 */
    u8  mRaycastLine;       /* 0x134 */
    u8  pad_135[0x83];
    s32 unk_1b8;            /* 0x1b8 */
};

#endif /* __cplusplus */

#endif /* WITHMESHCLSN_H */
