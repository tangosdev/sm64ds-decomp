/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BowserPuzzleManager: 5 matched functions, 14 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOWSERPUZZLEMANAGER_H
#define BOWSERPUZZLEMANAGER_H
#include "types.h"

struct BowserPuzzleManager {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x82];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x1c7];
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x37];
    s32 unk_324;            /* 0x324 */
    u8  unk_328;            /* 0x328 */
    u8  pad_329[0x3];
    s32 unk_32c;            /* 0x32c */
    u8  pad_330[0x4];
    s16 unk_334;            /* 0x334 */
    u8  unk_336;            /* 0x336 */
    u8  unk_337;            /* 0x337 */
    u8  unk_338;            /* 0x338 */
    u8  unk_339;            /* 0x339 */
    u8  unk_33a;            /* 0x33a */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int Render();
    void InitResources();
#endif
};

#endif
