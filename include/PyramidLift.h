/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PyramidLift: 5 matched functions, 17 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef PYRAMIDLIFT_H
#define PYRAMIDLIFT_H
#include "types.h"

struct PyramidLift {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x18];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x28];
    u8  mModel1;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1c7];
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x33];
    u8  mModel2;            /* 0x320 */
    u8  pad_321[0x1b];
    u8  unk_33c;            /* 0x33c */
    u8  pad_33d[0x33];
    s32 unk_370;            /* 0x370 */
    s32 unk_374;            /* 0x374 */
    s32 unk_378;            /* 0x378 */
    u8  pad_37c[0x78];
    u16 mShakeTimer;            /* 0x3f4 */
    u8  mState;            /* 0x3f6 */
    u8  unk_3f7;            /* 0x3f7 */
    u8  unk_3f8;            /* 0x3f8 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
