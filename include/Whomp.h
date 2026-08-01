/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Whomp: 6 matched functions, 35 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef WHOMP_H
#define WHOMP_H
#include "types.h"

struct Whomp {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    u8  pad_092[0x2];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x6];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x5c];
    u16 unk_100;            /* 0x100 */
    u8  pad_102[0xe];
    u8  mWithMeshClsn;            /* 0x110 */
    u8  pad_111[0x1bb];
    u8  mModelAnim;            /* 0x2cc */
    u8  pad_2cd[0x63];
    u8  mTextureSequence;            /* 0x330 */
    u8  pad_331[0x7];
    s32 unk_338;            /* 0x338 */
    s32 unk_33c;            /* 0x33c */
    u8  pad_340[0x4];
    u8  mShadowModel;            /* 0x344 */
    u8  pad_345[0x6b];
    s32 unk_3b0;            /* 0x3b0 */
    u8  pad_3b4[0x8];
    s32 unk_3bc;            /* 0x3bc */
    s32 unk_3c0;            /* 0x3c0 */
    s32 unk_3c4;            /* 0x3c4 */
    u8  pad_3c8[0x39];
    u8  unk_401;            /* 0x401 */
    u8  unk_402;            /* 0x402 */
    u8  unk_403;            /* 0x403 */
    u8  unk_404;            /* 0x404 */
    u8  pad_405[0x1];
    u8  unk_406;            /* 0x406 */
    u8  unk_407;            /* 0x407 */
    u8  unk_408;            /* 0x408 */
    u8  unk_409;            /* 0x409 */
    u8  pad_40a[0x1];
    u8  unk_40b;            /* 0x40b */
    u8  unk_40c;            /* 0x40c */
    u8  pad_40d[0x3];
    s32 unk_410;            /* 0x410 */
    u8  mIsKing;            /* 0x414 */
    u8  pad_415[0x3];
    u8  mMovingMeshCollider;            /* 0x418 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
