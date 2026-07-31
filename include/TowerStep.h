/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class TowerStep: 6 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TOWERSTEP_H
#define TOWERSTEP_H
#include "types.h"

struct TowerStep {
    u8  pad_000[0x74];
    u8  unk_074;            /* 0x074 */
    u8  pad_075[0x19];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x4];
    s16 unk_094;            /* 0x094 */
    u8  pad_096[0x3e];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1f9];
    s8  unk_31e;            /* 0x31e */
    u8  pad_31f[0x1];
    s32 unk_320;            /* 0x320 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
