/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Lakitu: 5 matched functions, 24 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef LAKITU_H
#define LAKITU_H
#include "types.h"

struct Lakitu {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    /* Actor::mPosX -- Actor.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 unk_05c;            /* 0x05c */
    /* Actor::mPosY -- Actor.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 unk_060;            /* 0x060 */
    /* 0x064..0x080 is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 unk_064;                 /* 0x064 */
    s32 unk_068;                 /* 0x068 */
    s32 unk_06c;                 /* 0x06c */
    s32 unk_070;                 /* 0x070 */
    s32 mCamSpacePosX;           /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x10];
    /* Actor::mVertAccel -- Actor.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 unk_09c;            /* 0x09c */
    /* 0x0a0..0x0b0 is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 unk_0a0;                 /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;              /* 0x0a8 */
    u8  pad_0ac[0x4];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x20];
    u8  mModelAnim;            /* 0x0d4 */
    u8  pad_0d5[0x7];
    u8  unk_0dc;            /* 0x0dc */
    u8  pad_0dd[0x4f];
    s32 unk_12c;            /* 0x12c */
    u8  pad_130[0x8];
    u8  mModel;            /* 0x138 */
    u8  pad_139[0x4f];
    u8  mShadowModel;            /* 0x188 */
    u8  pad_189[0x27];
    u8  mTextureSequence;            /* 0x1b0 */
    u8  pad_1b1[0x13];
    u8  mMovingCylinderClsnWithPos;            /* 0x1c4 */
    u8  pad_1c5[0x3f];
    u8  mWithMeshClsn;            /* 0x204 */
    u8  pad_205[0x1bb];
    u8  unk_3c0;            /* 0x3c0 */
    u8  pad_3c1[0x33];
    s32 unk_3f4;            /* 0x3f4 */
    u8  unk_3f8;            /* 0x3f8 */
    u8  pad_3f9[0x3];
    u8  unk_3fc;            /* 0x3fc */
    u8  pad_3fd[0x3];
    s32 unk_400;            /* 0x400 */
    u8  pad_404[0xc];
    s32 unk_410;            /* 0x410 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
