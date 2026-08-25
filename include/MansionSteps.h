/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MansionSteps: 5 matched functions, 6 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MANSIONSTEPS_H
#define MANSIONSTEPS_H
#include "types.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

struct MansionSteps {
    u8  pad_000[0xd4];
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
       checks. Was a u8 marker. [_ZN12MansionStepsD0Ev.c] */
    dBgW_KcMbg mMovingMeshCollider;            /* 0x15c */
    /* The collider's transform, the second half of the dBgW_KcMbg + Matrix4x3
       pair this tree carries everywhere a moving mesh collider appears
       (dBgActor_c 0x124/0x2ec, SpinningPlatform and TtcRotatingCube the same,
       both of which hand `this + 0x2ec` to dBgW_KcMbg::SetFile as a
       `const Matrix4x3 &`). 0x15c + 0x1c8 = 0x324, and 0x324 + 0x30 closes on
       the 0x354 MansionSteps_Spawn allocates. InitResources, which is where
       the SetFile call would be, is still a near miss and not in the tree. */
    Matrix4x3 mClsnMat;            /* 0x324 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int Render();
#endif
};

typedef char MansionSteps_size_must_be_0x354[sizeof(struct MansionSteps) == 0x354 ? 1 : -1];

#endif
