/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class StarMarker: 8 matched functions, 30 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef STARMARKER_H
#define STARMARKER_H
#include "types.h"

struct StarMarker {
    u8  pad_000[0x4];
    s32 unk_004;            /* 0x004 */
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x3c];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    u8  mMovingCylinderClsnWithPos;            /* 0x0d4 */
    u8  pad_0d5[0x1f];
    s32 unk_0f4;            /* 0x0f4 */
    s32 unk_0f8;            /* 0x0f8 */
    u8  pad_0fc[0x18];
    u8  mModel;            /* 0x114 */
    u8  pad_115[0x3f];
    s32 unk_154;            /* 0x154 */
    s32 unk_158;            /* 0x158 */
    s32 unk_15c;            /* 0x15c */
    u8  pad_160[0x4];
    u8  mShadowModel;            /* 0x164 */
    u8  pad_165[0x27];
    u8  unk_18c;            /* 0x18c */
    u8  pad_18d[0x23];
    s32 unk_1b0;            /* 0x1b0 */
    s32 unk_1b4;            /* 0x1b4 */
    s32 unk_1b8;            /* 0x1b8 */
    s32 unk_1bc;            /* 0x1bc */
    s32 unk_1c0;            /* 0x1c0 */
    s32 unk_1c4;            /* 0x1c4 */
    s32 unk_1c8;            /* 0x1c8 */
    s32 unk_1cc;            /* 0x1cc */
    s32 unk_1d0;            /* 0x1d0 */
    u16 mAppearTimer;            /* 0x1d4 */
    s16 unk_1d6;            /* 0x1d6 */
    u8  mState;            /* 0x1d8 */
    u8  mStarID;            /* 0x1d9 */
    u8  pad_1da[0x1];
    u8  mFlags;            /* 0x1db */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
    void OnPendingDestroy();
    void SpawnRedCoinStarIfNecessary();
#endif
};

#endif
