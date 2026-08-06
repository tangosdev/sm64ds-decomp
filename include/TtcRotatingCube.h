/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class TtcRotatingCube: 6 matched functions, 12 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TTCROTATINGCUBE_H
#define TTCROTATINGCUBE_H
#include "types.h"

struct TtcRotatingCube {
    u8  pad_000[0x90];
    /* 0x090..0x0a8 is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s16 unk_090;                 /* 0x090 */
    s16 mPrevAngleX;             /* 0x092 */
    s16 mPrevAngleY;             /* 0x094 */
    s16 mPrevAngleZ;             /* 0x096 */
    s32 mHorzSpeed;              /* 0x098 */
    s32 mVertAccel;              /* 0x09c */
    s32 mTerminalVelocity;       /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x4];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x20];
    u8  mModel1;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mModel2;            /* 0x320 */
    u8  pad_321[0x4f];
    s32 unk_370;            /* 0x370 */
    s16 mWaitTimer;            /* 0x374 */
    u8  unk_376;            /* 0x376 */
    u8  unk_377;            /* 0x377 */
    s16 unk_378;            /* 0x378 */
    u8  pad_37a[0x6];
    u8  mShadowModel;            /* 0x380 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
