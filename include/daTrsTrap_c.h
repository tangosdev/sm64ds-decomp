/* Seeded by tools/gen_header.py from matched-function evidence; the class name
 * and the five vtable slots below are ROM RTTI, not generated guesses.
 * daTrsTrap_c is shared by four actors -- MansionSteps, Bookshelf, MerryGoRound
 * and TrapDoor all store _ZTV11daTrsTrap_c @ 0x0211ea88 -- which is why the
 * one-actor spelling `MansionSteps` could never have been the class.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef DATRSTRAP_C_H
#define DATRSTRAP_C_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"
#include "dActor_c.h"

/* ROM RTTI and the destructor symbols authenticate daTrsTrap_c as the real
 * lifecycle type.  The extra word is evidenced by the first member beginning
 * at +0xd4 while dActor_c closes at +0xd0. */
struct daTrsTrap_c : dActor_c {
    u32 pad_0d0;
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    s32 unk_124;            /* 0x124 */
    u8  pad_128[0x18];
    s32 mIndex;            /* 0x140 */
    u8  pad_144[0xc];
    u8  mState;            /* 0x150 */
    u8  pad_151[0x5];
    u8  mVisible;            /* 0x156 */
    u8  pad_157[0x5];
    /* dBgW_KcMbg member, named by the class's own destructor calling
       dBgW_KcMbg's D1 at +0x15c -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN11daTrsTrap_cD0Ev.cpp] */
    dBgW_KcMbg mMovingMeshCollider;            /* 0x15c */
    /* The collider's transform, the second half of the dBgW_KcMbg + Matrix4x3
       pair this tree carries everywhere a moving mesh collider appears
       (dBgActor_c 0x124/0x2ec, SpinningPlatform and TtcRotatingCube the same,
       both of which hand `this + 0x2ec` to dBgW_KcMbg::SetFile as a
       `const Matrix4x3 &`). 0x15c + 0x1c8 = 0x324, and 0x324 + 0x30 closes on
       the 0x354 daTrsTrap_c_classInit_KAIDAN allocates. InitResources, which is where
       the SetFile call would be, is still a near miss and not in the tree. */
    Matrix4x3 mClsnMat;            /* 0x324 */

    /* --- vtable ---
     * Overrides of fBase_c virtuals, so each takes the base's slot whatever the
     * order here; the destructor stays first-declared because it is the ABI key
     * function and picks the TU that emits _ZTV11daTrsTrap_c. */
    virtual ~daTrsTrap_c();          /* slots 16 (D1), 17 (D0) */
    virtual s32 InitResources();     /* slot  0 -- ov063:0x0211cf00, still delinked */
    virtual s32 CleanupResources();  /* slot  3 -- ov063:0x0211cdec */
    virtual s32 Behavior();          /* slot  6 -- ov063:0x0211ce74 */
    virtual s32 Render();            /* slot  9 -- ov063:0x0211ce34 */
    virtual void OnPendingDestroy(); /* slot 12 -- ov063:0x0211ce30 */
};

typedef char daTrsTrap_c_size_must_be_0x354[sizeof(daTrsTrap_c) == 0x354 ? 1 : -1];

#endif
