/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MovingBarSmall: 6 matched functions, 21 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MOVINGBARSMALL_H
#define MOVINGBARSMALL_H
#include "types.h"

struct MovingBarSmall {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x4];
    s16 unk_094;            /* 0x094 */
    u8  pad_096[0x2];
    s32 unk_098;            /* 0x098 */
    u8  pad_09c[0x4];
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x28];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mShadowModel;            /* 0x320 */
    u8  pad_321[0x57];
    s32 unk_378;            /* 0x378 */
    s32 unk_37c;            /* 0x37c */
    s32 unk_380;            /* 0x380 */
    s32 unk_384;            /* 0x384 */
    s32 unk_388;            /* 0x388 */
    u8  unk_38c;            /* 0x38c */
    u8  pad_38d[0x3];
    u8  unk_390;            /* 0x390 */
    u8  unk_391;            /* 0x391 */
    u8  unk_392;            /* 0x392 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int Render();
#endif
};

#endif
