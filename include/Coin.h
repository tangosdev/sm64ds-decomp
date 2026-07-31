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
    u8  unk_05c;            /* 0x05c */
    u8  pad_05d[0x3];
    s32 mPosY;            /* 0x060 */
    u8  unk_064;            /* 0x064 */
    u8  pad_065[0xf];
    u8  unk_074;            /* 0x074 */
    u8  pad_075[0x1f];
    s16 unk_094;            /* 0x094 */
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
