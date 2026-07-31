/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class WaterfallMist: 7 matched functions, 31 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef WATERFALLMIST_H
#define WATERFALLMIST_H
#include "types.h"

struct WaterfallMist {
    u8  pad_000[0x8];
    u32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x10];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x43];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x3];
    s32 unk_114;            /* 0x114 */
    s32 unk_118;            /* 0x118 */
    u8  pad_11c[0x10];
    u8  unk_12c;            /* 0x12c */
    u8  pad_12d[0x17];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModelAnim;            /* 0x300 */
    u8  pad_301[0x5b];
    s32 unk_35c;            /* 0x35c */
    u8  pad_360[0x4];
    u8  mShadowModel;            /* 0x364 */
    u8  pad_365[0x57];
    s32 unk_3bc;            /* 0x3bc */
    s32 unk_3c0;            /* 0x3c0 */
    s32 unk_3c4;            /* 0x3c4 */
    s32 unk_3c8;            /* 0x3c8 */
    s32 unk_3cc;            /* 0x3cc */
    u8  unk_3d0;            /* 0x3d0 */
    u8  pad_3d1[0x1b];
    s32 unk_3ec;            /* 0x3ec */
    s32 mType;            /* 0x3f0 */
    s32 mModelIndex;            /* 0x3f4 */
    u8  pad_3f8[0x7];
    u8  unk_3ff;            /* 0x3ff */
    u8  unk_400;            /* 0x400 */
    u8  unk_401;            /* 0x401 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    void OnPendingDestroy();
#endif
};

#endif
