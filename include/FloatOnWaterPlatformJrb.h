/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class FloatOnWaterPlatformJrb: 6 matched functions, 23 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FLOATONWATERPLATFORMJRB_H
#define FLOATONWATERPLATFORMJRB_H
#include "types.h"

struct FloatOnWaterPlatformJrb {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    s16 unk_090;            /* 0x090 */
    u8  pad_092[0x2];
    s16 unk_094;            /* 0x094 */
    u8  pad_096[0x2];
    s32 unk_098;            /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    s32 unk_0a4;            /* 0x0a4 */
    s32 unk_0a8;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    u8  pad_0b0[0x24];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    s32 unk_320;            /* 0x320 */
    u8  mWithMeshClsn;            /* 0x324 */
    u8  pad_325[0x1bb];
    s32 unk_4e0;            /* 0x4e0 */
    s32 unk_4e4;            /* 0x4e4 */
    s32 unk_4e8;            /* 0x4e8 */
    s32 unk_4ec;            /* 0x4ec */
    s32 unk_4f0;            /* 0x4f0 */
    u8  unk_4f4;            /* 0x4f4 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
