/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class CannonHatch: 6 matched functions, 14 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CANNONHATCH_H
#define CANNONHATCH_H
#include "types.h"

struct CannonHatch {
    u8  pad_000[0x8];
    u8  mParam;            /* 0x008 */
    u8  pad_009[0x53];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1c7];
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x33];
    s32 unk_320;            /* 0x320 */
    s32 unk_324;            /* 0x324 */
    s32 unk_328;            /* 0x328 */
    u8  unk_32c;            /* 0x32c */
    u8  unk_32d;            /* 0x32d */
    u8  unk_32e;            /* 0x32e */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
