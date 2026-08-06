/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PiranhaPlant: 6 matched functions, 39 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef PIRANHAPLANT_H
#define PIRANHAPLANT_H
#include "types.h"

struct PiranhaPlant {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    u16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x1a];
    /* 0x0b0..0x100 is Actor's, and Actor.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    u32 unk_0b0;                 /* 0x0b0 */
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
    s16 unk_100;            /* 0x100 */
    u8  pad_102[0x6];
    u8  unk_108;            /* 0x108 */
    u8  pad_109[0x7];
    u8  mModelAnim;            /* 0x110 */
    u8  pad_111[0x4f];
    u8  mAnimation;            /* 0x160 */
    u8  pad_161[0xf];
    s32 unk_170;            /* 0x170 */
    u8  mModel;            /* 0x174 */
    u8  pad_175[0x4f];
    u8  mWithMeshClsn;            /* 0x1c4 */
    u8  pad_1c5[0x1bb];
    u8  mMovingCylinderClsn1;            /* 0x380 */
    u8  pad_381[0x33];
    u8  mMovingCylinderClsn2;            /* 0x3b4 */
    u8  pad_3b5[0x33];
    u8  mMovingCylinderClsnWithPos;            /* 0x3e8 */
    u8  pad_3e9[0x3f];
    u8  unk_428;            /* 0x428 */
    u8  pad_429[0xb];
    s32 unk_434;            /* 0x434 */
    s32 unk_438;            /* 0x438 */
    s32 unk_43c;            /* 0x43c */
    s32 unk_440;            /* 0x440 */
    s32 unk_444;            /* 0x444 */
    s32 unk_448;            /* 0x448 */
    s32 unk_44c;            /* 0x44c */
    s32 unk_450;            /* 0x450 */
    s32 unk_454;            /* 0x454 */
    s32 mState;            /* 0x458 */
    u8  unk_45c;            /* 0x45c */
    u8  unk_45d;            /* 0x45d */
    u8  pad_45e[0x2];
    s32 unk_460;            /* 0x460 */
    s32 unk_464;            /* 0x464 */
    s16 unk_468;            /* 0x468 */
    u8  pad_46a[0x2];
    s32 unk_46c;            /* 0x46c */
    s32 unk_470;            /* 0x470 */
    s32 unk_474;            /* 0x474 */
    s32 unk_478;            /* 0x478 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
