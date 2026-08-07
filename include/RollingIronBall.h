/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RollingIronBall: 6 matched functions, 31 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ROLLINGIRONBALL_H
#define ROLLINGIRONBALL_H
#include "types.h"
#include "Model.h"

struct RollingIronBall {
    u8  pad_000[0x8];
    u32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x2c];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x2];
    s32 mHorzSpeed;            /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x5c];
    s16 unk_100;            /* 0x100 */
    u8  pad_102[0x6];
    u8  unk_108;            /* 0x108 */
    u8  pad_109[0x7];
    u8  mWithMeshClsn;            /* 0x110 */
    u8  pad_111[0x1bb];
    /* Model member, named by _ZN5ModelD1Ev at +0x2cc -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x2cc */
    u8  mShadowModel;            /* 0x31c */
    u8  pad_31d[0x57];
    u8  mMovingCylinderClsn;            /* 0x374 */
    u8  pad_375[0x33];
    s32 unk_3a8;            /* 0x3a8 */
    s32 unk_3ac;            /* 0x3ac */
    s32 unk_3b0;            /* 0x3b0 */
    s32 unk_3b4;            /* 0x3b4 */
    u8  pad_3b8[0x2];
    s16 unk_3ba;            /* 0x3ba */
    u8  pad_3bc[0x4];
    s32 unk_3c0;            /* 0x3c0 */
    s32 unk_3c4;            /* 0x3c4 */
    s32 unk_3c8;            /* 0x3c8 */
    s32 unk_3cc;            /* 0x3cc */
    u8  unk_3d0;            /* 0x3d0 */
    u8  pad_3d1[0x1];
    u8  unk_3d2;            /* 0x3d2 */
    u8  pad_3d3[0x1];
    s32 unk_3d4;            /* 0x3d4 */
    s32 unk_3d8;            /* 0x3d8 */
    s32 unk_3dc;            /* 0x3dc */
    s32 unk_3e0;            /* 0x3e0 */
    s32 unk_3e4;            /* 0x3e4 */
    u8  pad_3e8[0xc];
    u8  mPathPtr;            /* 0x3f4 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
