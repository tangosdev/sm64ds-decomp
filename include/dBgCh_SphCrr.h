/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class dBgCh_SphCrr: 4 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * PROMOTED 2026-08-23 to a real MI declaration (notes/ctor-migration.md item
 * 2). The ROM's own RTTI record states the base list outright:
 *
 *     dBgCh    @ 0x00   polymorphic   (vptr store at +0)
 *     dBgPi    @ 0x10   polymorphic   (secondary block VTable_dBgPi_dBgCh_SphCrrThunk)
 *     dM3dGSph @ 0x38   polymorphic   (secondary block VTable_dM3dGSph_dBgCh_SphCrrThunk;
 *                                     this IS the query sphere below)
 *
 * The C++ branch declares exactly that; the C branch below stays field-for-
 * field IDENTICAL to what this header carried before promotion, because .c
 * translation units reach into these interiors by the old member names.
 */
#ifndef DBGCH_SPHCRR_H
#define DBGCH_SPHCRR_H
#include "types.h"
#include "dBgPi.h"

#ifdef __cplusplus

#include "math/Fix12.h"
#include "dBgCh.h"
#include "dM3dGSph.h"

/* fwd */
struct dActor_c;

struct dBgCh_SphCrr : dBgCh, dBgPi, dM3dGSph {
    /* 0x38..0x4b is the dM3dGSph base itself: vptr at 0x38, centre (pos) at
       0x3c, radius at 0x48 -- see include/dM3dGSph.h.

       THE QUERY SPHERE. dBgW_KcMbg::DetectClsn(dBgCh_SphCrr&) hands 0x03c
       to func_02039e48, which transforms a Vector3 into the collider's local
       frame, and scales 0x048 by the collider's uniform scale before passing it
       as the `Fix12<int> radius` argument of dBgCh_SphCrr::SetObjAndSphere -- so
       0x03c is twelve bytes of centre and 0x048 is the radius that follows it. */
    /* A Vector3 DISPLACEMENT. dBgCh_Actr's Update* methods read these three
       words (their Actr +0x6c) as a Vector3 and feed them to the movement
       helpers; named when Actr's flat blob was typed out. */
    Vector3 disp;           /* 0x04c */
    u8  pad_058[0x18];      /* through 0x06f */
    /* Result flags, read and OR-ed a bit at a time by the same function:
       1 = any hit, 4 = floor, 8 = wall, 0x10 = from underneath. Each bit gates
       copying the matching dBgPi below it. */
    u8  flags;              /* 0x070 */
    u8  pad_071[0x3];
    dBgPi mClsnResult1;     /* 0x074 */
    dBgPi mClsnResult2;     /* 0x09c */
    dBgPi mClsnResult3;     /* 0x0c4 */
    s32 unk_0ec;            /* 0x0ec */
    u8  pad_0f0[0xc];
    /* An adjacent pair, kept unnamed. All that is evidenced is the shape of
       their use in dBgW_KcMbg::DetectClsn(dBgCh_SphCrr&): 0x100 is
       compared `<` against the local query's own 0x100, and on winning, 0x0fc
       is handed BY ADDRESS to func_0203794c -- i.e. a score at 0x100 selecting
       a payload that starts at 0x0fc. What the payload is is not settled here. */
    s32 unk_0fc;            /* 0x0fc */
    s32 unk_100;            /* 0x100 */
    u8  pad_104[0x4];       /* through 0x10b */
    s32 mScale;            /* 0x108 - dBgCh_Actr's Update* copy its tail word
                               (Actr +0x128) here each update */
    s32 unk_10c;            /* 0x10c - named 2026-08-24 when the size pin
                               landed: dBgCh_Actr::Init stores its fourth
                               argument, a Vector3_16 *, at Actr +0x12c = this
                               +0x10c, and UpdateContinuous forwards both it
                               and the next word to func_02038324 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* DECLARED FIRST AND NEVER DEFINED AS A METHOD -- the key-function
     * arrangement from include/ModelBase.h / include/dBgCh.h. With MI this is
     * doubly load-bearing: a TU that DID define it out of line would emit both
     * vtable blocks AND the _ZTh thunks, all of which the ROM already supplies
     * as data (see include/ModelAnim.h for that exact trap).
     */
    virtual ~dBgCh_SphCrr();

    /* DECLARED, defined out of line in src/_ZN12dBgCh_SphCrrC1Ev.cpp as real
     * C++ -- complete-object context for every ROM caller, hence C1.
     */
    dBgCh_SphCrr();

    /* methods */
    void SetFloorResult(const dBgPi & src_);
    void SetObjAndSphere(const Vector3 &pos, Fix12<int> radius, dActor_c *actor);
};

/* SIZE PINNED AT 0x110 by dBgW_KcMbg::DetectClsn(dBgCh_SphCrr&): the ROM gives
   that function's local query object an exact 0x110 stack slot, and compiling
   it against a 0x10c declaration came out one word short of the frame. That
   measurement is what let the byte stand-in there stay honest; growing this
   class is what will eventually let the stand-in become the real type. */
typedef char dBgCh_SphCrr_size_must_be_0x110[
    sizeof(dBgCh_SphCrr) == 0x110 ? 1 : -1];

#else

struct dBgCh_SphCrr {
    u8  pad_000[0x10];
    u8  mBgPiBase;            /* 0x010 */
    u8  pad_011[0x27];
    u8  mSphereBase;            /* 0x038 */
    u8  pad_039[0x3];
    Vector3 pos;            /* 0x03c */
    Fix12i radius;          /* 0x048 */
    u8  pad_04c[0x24];
    u8  flags;              /* 0x070 - see the C++ branch; Actr's accessors
                               call this byte their mClsnFlags home */
    u8  pad_071[0x3];
    struct dBgPi mClsnResult1; /* 0x074 */
    struct dBgPi mClsnResult2; /* 0x09c */
    struct dBgPi mClsnResult3; /* 0x0c4 */
    s32 unk_0ec;            /* 0x0ec */
    u8  pad_0f0[0xc];
    s32 unk_0fc;            /* 0x0fc */
    s32 unk_100;            /* 0x100 */
    u8  pad_104[0x4];
    s32 mScale;            /* 0x108 */
    s32 unk_10c;            /* 0x10c - see the C++ branch: Init's Vector3_16 * */
};

typedef struct dBgCh_SphCrr dBgCh_SphCrr;

/* Same pin on the C view -- the two branches must agree while anything can
   still substitute one for the other. */
typedef char dBgCh_SphCrr_c_size_must_be_0x110[
    sizeof(struct dBgCh_SphCrr) == 0x110 ? 1 : -1];

#endif /* __cplusplus */

#endif
