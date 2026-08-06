/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Unagi: 5 matched functions, 38 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef UNAGI_H
#define UNAGI_H
#include "types.h"

struct Unagi {
    u8  pad_000[0x8];
    u32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    s16 mPrevAngleX;            /* 0x092 */
    s16 mPrevAngleY;            /* 0x094 */
    s16 mPrevAngleZ;            /* 0x096 */
    u8  pad_098[0x8];
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x28];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x33];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0xf];
    u8  mMovingCylinderClsnWithPos1;            /* 0x110 */
    u8  pad_111[0x3f];
    u8  mMovingCylinderClsnWithPos2;            /* 0x150 */
    u8  pad_151[0x3f];
    u8  mWithMeshClsn;            /* 0x190 */
    u8  pad_191[0x1bb];
    s32 unk_34c;            /* 0x34c */
    u8  mBlendModelAnim;            /* 0x350 */
    u8  pad_351[0xf];
    s32 unk_360;            /* 0x360 */
    u8  pad_364[0x48];
    s32 unk_3ac;            /* 0x3ac */
    u8  pad_3b0[0x4];
    s32 unk_3b4;            /* 0x3b4 */
    u8  pad_3b8[0x38];
    s32 unk_3f0;            /* 0x3f0 */
    s32 unk_3f4;            /* 0x3f4 */
    s32 unk_3f8;            /* 0x3f8 */
    u8  pad_3fc[0x8];
    s32 mPathID;            /* 0x404 */
    s32 mVariant;            /* 0x408 */
    s32 unk_40c;            /* 0x40c */
    s32 unk_410;            /* 0x410 */
    u8  unk_414;            /* 0x414 */
    u8  pad_415[0x3];
    u8  unk_418;            /* 0x418 */
    u8  pad_419[0xd];
    u8  unk_426;            /* 0x426 */
    u8  pad_427[0x1];
    s16 unk_428;            /* 0x428 */
    s16 unk_42a;            /* 0x42a */
    s16 unk_42c;            /* 0x42c */
    u8  pad_42e[0xe];
    s32 unk_43c;            /* 0x43c */
    s32 unk_440;            /* 0x440 */
    s32 unk_444;            /* 0x444 */
    u8  pad_448[0x54];
    s32 mStarUniqueID;            /* 0x49c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
