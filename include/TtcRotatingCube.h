/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class TtcRotatingCube: 6 matched functions, 12 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TTCROTATINGCUBE_H
#define TTCROTATINGCUBE_H
#include "types.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dBgW_KcMbg.h"

struct TtcRotatingCube {
    u8  pad_000[0x90];
    /* 0x090..0x0a8 is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s16 mAngleZ;                 /* 0x090 */
    s16 mPrevAngleX;             /* 0x092 */
    s16 mPrevAngleY;             /* 0x094 */
    s16 mPrevAngleZ;             /* 0x096 */
    s32 mHorzSpeed;              /* 0x098 */
    s32 mVertAccel;              /* 0x09c */
    s32 mTerminalVelocity;       /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;            /* 0x0a8 */
    u8  pad_0ac[0x4];
    s32 mFlags;            /* 0x0b0 */
    u8  pad_0b4[0x20];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel1;            /* 0x0d4 */
    /* dBgW_KcMbg member, named by the class's own destructor calling
       dBgW_KcMbg's D1 at +0x124 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN15TtcRotatingCubeD1Ev.c] */
    dBgW_KcMbg mMovingMeshCollider;            /* 0x124 */
    /* The collider's transform: InitResources hands +0x2ec to
       dBgW_KcMbg::SetFile as its `const Matrix4x3 &`. */
    Matrix4x3 mClsnMat;            /* 0x2ec */
    u8  pad_31c[0x4];
    /* Model member, named by _ZN5ModelD1Ev at +0x320 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel2;            /* 0x320 */
    s32 unk_370;            /* 0x370 */
    s16 mWaitTimer;            /* 0x374 */
    u8  unk_376;            /* 0x376 */
    u8  unk_377;            /* 0x377 */
    s16 unk_378;            /* 0x378 */
    /* Set to 1 by InitResources when the two ground probes disagree. */
    u8  unk_37a;            /* 0x37a */
    u8  pad_37b[0x1];
    /* The floor height under the cube, from a dBgCh_Gnd probe. */
    s32 unk_37c;            /* 0x37c */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x380 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN15TtcRotatingCubeD1Ev.c] */
    ShadowModel mShadowModel;            /* 0x380 */
    /* The shadow's transform, the same ShadowModel + Matrix4x3 pair
       HauntedChair evidences by byte. 0x3a8 + 0x30 closes on the 0x3d8
       TtcRotatingCube_Spawn allocates. */
    Matrix4x3 mShadowMat;            /* 0x3a8 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char TtcRotatingCube_size_must_be_0x3d8[
    sizeof(struct TtcRotatingCube) == 0x3d8 ? 1 : -1];

#endif
