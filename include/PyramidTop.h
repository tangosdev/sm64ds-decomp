/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PyramidTop: 6 matched functions, 17 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef PYRAMIDTOP_H
#define PYRAMIDTOP_H
#include "types.h"

struct PyramidTop {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    u8  unk_074;            /* 0x074 */
    u8  pad_075[0x19];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel1;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mModel2;            /* 0x320 */
    u8  pad_321[0x7f];
    s32 unk_3a0;            /* 0x3a0 */
    s32 unk_3a4;            /* 0x3a4 */
    s32 unk_3a8;            /* 0x3a8 */
    s32 unk_3ac;            /* 0x3ac */
    s16 unk_3b0;            /* 0x3b0 */
    u16 unk_3b2;            /* 0x3b2 */
    s16 unk_3b4;            /* 0x3b4 */
    u8  unk_3b6;            /* 0x3b6 */
    u8  unk_3b7;            /* 0x3b7 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
