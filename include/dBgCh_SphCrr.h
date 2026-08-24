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
    u8  pad_04c[0x24];
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
    u8  pad_104[0x4];       /* through 0x10b; the stand-in in KcMbg's DetectClsn
                               spans 0x110, so one more word may belong here --
                               unsettled, and nothing depends on it yet */
    s32 unk_108;            /* 0x108 */

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

#else

struct dBgCh_SphCrr {
    u8  pad_000[0x10];
    u8  unk_010;            /* 0x010 */
    u8  pad_011[0x27];
    u8  unk_038;            /* 0x038 */
    u8  pad_039[0x3];
    Vector3 pos;            /* 0x03c */
    Fix12i radius;          /* 0x048 */
    u8  pad_04c[0x24];
    u8  flags;              /* 0x070 */
    u8  pad_071[0x3];
    struct dBgPi mClsnResult1; /* 0x074 */
    struct dBgPi mClsnResult2; /* 0x09c */
    struct dBgPi mClsnResult3; /* 0x0c4 */
    s32 unk_0ec;            /* 0x0ec */
    u8  pad_0f0[0xc];
    s32 unk_0fc;            /* 0x0fc */
    s32 unk_100;            /* 0x100 */
    u8  pad_104[0x4];
    s32 unk_108;            /* 0x108 */
};

typedef struct dBgCh_SphCrr dBgCh_SphCrr;

#endif /* __cplusplus */

#endif
