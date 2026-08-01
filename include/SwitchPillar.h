/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SwitchPillar: 6 matched functions, 12 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SWITCHPILLAR_H
#define SWITCHPILLAR_H
#include "types.h"

struct SwitchPillar {
    u8  pad_000[0x60];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x10];
    u8  unk_074;            /* 0x074 */
    u8  pad_075[0x19];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x7];
    u8  unk_0dc;            /* 0x0dc */
    u8  pad_0dd[0x47];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mTextureTransformer;            /* 0x320 */
    u8  pad_321[0xb];
    s32 unk_32c;            /* 0x32c */
    u8  pad_330[0x4];
    s32 mLoweredY;            /* 0x334 */
    u32 unk_338;            /* 0x338 */
    u8  unk_33c;            /* 0x33c */
    u8  pad_33d[0x1];
    u8  unk_33e;            /* 0x33e */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
