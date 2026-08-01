/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RollingRock: 5 matched functions, 22 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ROLLINGROCK_H
#define ROLLINGROCK_H
#include "types.h"

struct RollingRock {
    u8  pad_000[0x4];
    u32 unk_004;            /* 0x004 */
    u8  pad_008[0x54];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    u8  unk_08c;            /* 0x08c */
    u8  pad_08d[0x1];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x2];
    u8  unk_092;            /* 0x092 */
    u8  pad_093[0x1];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x2];
    s32 unk_098;            /* 0x098 */
    u8  pad_09c[0x30];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x33];
    u16 unk_100;            /* 0x100 */
    u8  pad_102[0xa];
    s32 unk_10c;            /* 0x10c */
    u8  mModel;            /* 0x110 */
    u8  pad_111[0x4f];
    u8  mShadowModel;            /* 0x160 */
    u8  pad_161[0x57];
    u8  mMovingCylinderClsnWithPos;            /* 0x1b8 */
    u8  pad_1b9[0x3f];
    u8  mWithMeshClsn;            /* 0x1f8 */
    u8  pad_1f9[0x1bb];
    u32 unk_3b4;            /* 0x3b4 */
    u8  pad_3b8[0x6];
    u8  mType;            /* 0x3be */
    u8  unk_3bf;            /* 0x3bf */
    u8  unk_3c0;            /* 0x3c0 */
    u8  unk_3c1;            /* 0x3c1 */
    u8  unk_3c2;            /* 0x3c2 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
