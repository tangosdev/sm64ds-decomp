/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RotatingUpDownPlatform: 6 matched functions, 17 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ROTATINGUPDOWNPLATFORM_H
#define ROTATINGUPDOWNPLATFORM_H
#include "types.h"

struct RotatingUpDownPlatform {
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
    u8  pad_125[0x1ff];
    s32 unk_324;            /* 0x324 */
    s32 unk_328;            /* 0x328 */
    s32 unk_32c;            /* 0x32c */
    s32 unk_330;            /* 0x330 */
    s32 unk_334;            /* 0x334 */
    u8  pad_338[0xc];
    u8  mPathPtr;            /* 0x344 */
    u8  pad_345[0x7];
    s32 unk_34c;            /* 0x34c */
    s16 unk_350;            /* 0x350 */
    u8  mVariant;            /* 0x352 */
    u8  pad_353[0x3];
    u8  unk_356;            /* 0x356 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
