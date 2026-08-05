/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Eyerok: 6 matched functions, 22 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef EYEROK_H
#define EYEROK_H
#include "types.h"

struct Eyerok {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel1;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  unk_124;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mMovingCylinderClsnWithPos;            /* 0x320 */
    u8  pad_321[0x33];
    s32 unk_354;            /* 0x354 */
    s32 unk_358;            /* 0x358 */
    s32 unk_35c;            /* 0x35c */
    u8  mBlendModelAnim;            /* 0x360 */
    u8  pad_361[0x6f];
    u8  mModel2;            /* 0x3d0 */
    u8  pad_3d1[0x4f];
    u8  mShadowModel;            /* 0x420 */
    u8  pad_421[0x27];
    u8  mTextureSequence;            /* 0x448 */
    u8  pad_449[0x43];
    u8  unk_48c;            /* 0x48c */
    u8  pad_48d[0xf];
    s32 unk_49c;            /* 0x49c */
    u8  pad_4a0[0x8];
    s32 unk_4a8;            /* 0x4a8 */
    u8  pad_4ac[0x8];
    s32 unk_4b4;            /* 0x4b4 */
    u8  pad_4b8[0x18];
    u8  unk_4d0;            /* 0x4d0 */
    u8  pad_4d1[0x1];
    s16 unk_4d2;            /* 0x4d2 */
    u16 unk_4d4;            /* 0x4d4 */
    u8  pad_4d6[0x19e];
    u8  unk_674;            /* 0x674 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
