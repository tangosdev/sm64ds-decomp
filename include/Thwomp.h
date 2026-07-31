/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Thwomp: 6 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef THWOMP_H
#define THWOMP_H
#include "types.h"

struct Thwomp {
    u8  pad_000[0xd4];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    s32 mFileTable;            /* 0x320 */
    u8  mTextureSequence;            /* 0x324 */
    u8  pad_325[0x7];
    s32 unk_32c;            /* 0x32c */
    u8  pad_330[0x8];
    u8  mShadowModel;            /* 0x338 */
    u8  pad_339[0x5f];
    s32 mState;            /* 0x398 */
    u8  pad_39c[0x2];
    u8  unk_39e;            /* 0x39e */
    u8  pad_39f[0x3];
    u8  mTriggered;            /* 0x3a2 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
