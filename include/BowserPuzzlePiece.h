/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BowserPuzzlePiece: 4 matched functions, 12 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOWSERPUZZLEPIECE_H
#define BOWSERPUZZLEPIECE_H
#include "types.h"

struct BowserPuzzlePiece {
    u8  pad_000[0x8];
    u32 unk_008;            /* 0x008 */
    u8  pad_00c[0x80];
    /* Actor::mAngleX -- Actor.h declares s16 here, and it is de-bannered (hand-reconstructed). */
    s16 unk_08c;            /* 0x08c */
    /* Actor::mAngleY -- Actor.h declares s16 here, and it is de-bannered (hand-reconstructed). */
    s16 unk_08e;            /* 0x08e */
    /* Actor::mAngleZ -- Actor.h declares s16 here, and it is de-bannered (hand-reconstructed). */
    s16 unk_090;            /* 0x090 */
    /* Actor::mPrevAngleX -- Actor.h declares s16 here, and it is de-bannered (hand-reconstructed). */
    s16 unk_092;            /* 0x092 */
    /* Actor::mPrevAngleY -- Actor.h declares s16 here, and it is de-bannered (hand-reconstructed). */
    s16 unk_094;            /* 0x094 */
    /* 0x096..0x100 is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s16 unk_096;                 /* 0x096 */
    s32 mHorzSpeed;              /* 0x098 */
    s32 mVertAccel;              /* 0x09c */
    s32 mTerminalVelocity;       /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;              /* 0x0a8 */
    u8  pad_0ac[0x4];
    u32 mFlags;                  /* 0x0b0 */
    s32 unk_0b4;                 /* 0x0b4 */
    s32 unk_0b8;                 /* 0x0b8 */
    s32 unk_0bc;                 /* 0x0bc */
    s32 unk_0c0;                 /* 0x0c0 */
    u8  unk_0c4;                 /* 0x0c4 */
    u8  pad_0c5[0x7];
    s8  mAreaId;                 /* 0x0cc */
    u8  pad_0cd[0x1];
    s16 unk_0ce;                 /* 0x0ce */
    u8  pad_0d0[0x30];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0xf];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1cf];
    s32 unk_314;            /* 0x314 */
    s32 unk_318;            /* 0x318 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
