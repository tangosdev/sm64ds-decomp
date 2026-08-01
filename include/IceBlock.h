/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class IceBlock: 6 matched functions, 14 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ICEBLOCK_H
#define ICEBLOCK_H
#include "types.h"

struct IceBlock {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mMovingCylinderClsn;            /* 0x320 */
    u8  pad_321[0x1f];
    s32 unk_340;            /* 0x340 */
    s32 unk_344;            /* 0x344 */
    u8  pad_348[0xc];
    u8  unk_354;            /* 0x354 */
    u8  pad_355[0x3];
    s32 mScale;            /* 0x358 */
    u32 unk_35c;            /* 0x35c */
    u32 unk_360;            /* 0x360 */
    u8  unk_364;            /* 0x364 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
