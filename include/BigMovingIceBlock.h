/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BigMovingIceBlock: 6 matched functions, 11 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BIGMOVINGICEBLOCK_H
#define BIGMOVINGICEBLOCK_H
#include "types.h"

struct BigMovingIceBlock {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x8];
    s32 mHorzSpeed;            /* 0x098 */
    u8  pad_09c[0x38];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mPath;            /* 0x320 */
    u8  pad_321[0x7];
    s32 mPathNodeIdx;            /* 0x328 */
    s32 mPathDir;            /* 0x32c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
