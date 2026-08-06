/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Tornado: 5 matched functions, 25 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TORNADO_H
#define TORNADO_H
#include "types.h"

struct Tornado {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    /* 0x080..0x09c is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;                 /* 0x08e */
    s16 mAngleZ;                 /* 0x090 */
    s16 mPrevAngleX;             /* 0x092 */
    s16 mPrevAngleY;             /* 0x094 */
    s16 mPrevAngleZ;             /* 0x096 */
    s32 mHorzSpeed;              /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x30];
    u8  mMovingCylinderClsn;            /* 0x0d4 */
    u8  pad_0d5[0x1f];
    s32 unk_0f4;            /* 0x0f4 */
    u32 unk_0f8;            /* 0x0f8 */
    u8  pad_0fc[0xc];
    u8  mWithMeshClsn;            /* 0x108 */
    u8  pad_109[0x1bb];
    u8  mModelAnim;            /* 0x2c4 */
    u8  pad_2c5[0x4f];
    u8  mAnimation;            /* 0x314 */
    u8  pad_315[0x13];
    u8  mTextureTransformer;            /* 0x328 */
    u8  pad_329[0x13];
    s32 unk_33c;            /* 0x33c */
    s32 unk_340;            /* 0x340 */
    s32 unk_344;            /* 0x344 */
    s32 unk_348;            /* 0x348 */
    s32 unk_34c;            /* 0x34c */
    u16 unk_350;            /* 0x350 */
    u8  pad_352[0x8];
    s16 unk_35a;            /* 0x35a */
    s32 unk_35c;            /* 0x35c */
    u8  unk_360;            /* 0x360 */
    u8  pad_361[0x3];
    s32 unk_364;            /* 0x364 */
    s32 unk_368;            /* 0x368 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
