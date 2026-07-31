/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ChainChompFence: 6 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CHAINCHOMPFENCE_H
#define CHAINCHOMPFENCE_H
#include "types.h"

struct ChainChompFence {
    u8  pad_000[0x8e];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x1f9];
    u8  unk_31e;            /* 0x31e */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
