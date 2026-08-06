/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Coin: 6 matched functions, 29 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef COIN_H
#define COIN_H
#include "types.h"

struct Coin {
    u8  pad_000[0x8];
    u32 mParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    /* Actor::mPosX -- Actor.h declares s32 here, and it is de-bannered (hand-reconstructed). */
    s32 unk_05c;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    /* 0x064..0x074 is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 unk_064;                 /* 0x064 */
    s32 unk_068;                 /* 0x068 */
    s32 unk_06c;                 /* 0x06c */
    s32 unk_070;                 /* 0x070 */
    /* 0x074..0x094 is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 unk_074;                 /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;                 /* 0x08e */
    s16 mAngleZ;                 /* 0x090 */
    s16 mPrevAngleX;             /* 0x092 */
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x6];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x4];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x3];
    s32 mEatingPlayer;            /* 0x0d0 */
    s32 unk_0d4;            /* 0x0d4 */
    u8  mCommonModel1;            /* 0x0d8 */
    u8  pad_0d9[0x3b];
    u8  mCommonModel2;            /* 0x114 */
    u8  pad_115[0x3b];
    u8  mShadowModel;            /* 0x150 */
    u8  pad_151[0x27];
    u8  mCylinderClsn;            /* 0x178 */
    u8  pad_179[0x33];
    u8  mWithMeshClsn;            /* 0x1ac */
    u8  pad_1ad[0x1bb];
    u8  unk_368;            /* 0x368 */
    u8  pad_369[0x2f];
    s32 unk_398;            /* 0x398 */
    u8  pad_39c[0x4];
    s32 mCoinType;            /* 0x3a0 */
    s32 mBehaviorType;            /* 0x3a4 */
    s16 unk_3a8;            /* 0x3a8 */
    u8  unk_3aa;            /* 0x3aa */
    u8  unk_3ab;            /* 0x3ab */
    s8  unk_3ac;            /* 0x3ac */
    u8  pad_3ad[0x1];
    u8  unk_3ae;            /* 0x3ae */
    u8  pad_3af[0x1];
    u8  unk_3b0;            /* 0x3b0 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int Render();
#endif
};

#endif
