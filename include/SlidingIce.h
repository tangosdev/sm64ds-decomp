/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SlidingIce: 6 matched functions, 15 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SLIDINGICE_H
#define SLIDINGICE_H
#include "types.h"

struct SlidingIce {
    u8  pad_000[0xc];
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x2];
    s32 unk_098;            /* 0x098 */
    u8  pad_09c[0x30];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x4b];
    s8  unk_170;            /* 0x170 */
    u8  pad_171[0x1ad];
    s16 unk_31e;            /* 0x31e */
    s8  unk_320;            /* 0x320 */
    u8  pad_321[0x3];
    s32 unk_324;            /* 0x324 */
    s32 unk_328;            /* 0x328 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int Render();
#endif
};

#endif
