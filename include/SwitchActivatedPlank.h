/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SwitchActivatedPlank: 6 matched functions, 8 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SWITCHACTIVATEDPLANK_H
#define SWITCHACTIVATEDPLANK_H
#include "types.h"

struct SwitchActivatedPlank {
    u8  pad_000[0x8e];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    u8  mModel1;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mModel2;            /* 0x320 */
    u8  pad_321[0x7f];
    s16 unk_3a0;            /* 0x3a0 */
    u8  unk_3a2;            /* 0x3a2 */
    u8  unk_3a3;            /* 0x3a3 */
    u8  unk_3a4;            /* 0x3a4 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
