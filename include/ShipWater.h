/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ShipWater: 6 matched functions, 10 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SHIPWATER_H
#define SHIPWATER_H
#include "types.h"

struct ShipWater {
    u8  pad_000[0x60];
    s32 mPosY;            /* 0x060 */
    u8  pad_064[0x2a];
    u8  unk_08e;            /* 0x08e */
    u8  pad_08f[0x45];
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
    s32 unk_334;            /* 0x334 */
    u8  unk_338;            /* 0x338 */
    u8  pad_339[0x3];
    s32 unk_33c;            /* 0x33c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
