/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BowserFireSeaArena: 6 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOWSERFIRESEAARENA_H
#define BOWSERFIRESEAARENA_H
#include "types.h"

struct BowserFireSeaArena {
    u8  pad_000[0x8e];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel1;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMovingMeshCollider1;            /* 0x124 */
    u8  pad_125[0x1f9];
    s16 unk_31e;            /* 0x31e */
    s16 unk_320;            /* 0x320 */
    s16 unk_322;            /* 0x322 */
    u8  mModel2;            /* 0x324 */
    u8  pad_325[0x4f];
    u8  mMovingMeshCollider2;            /* 0x374 */
    u8  pad_375[0x1f7];
    s32 unk_56c;            /* 0x56c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
