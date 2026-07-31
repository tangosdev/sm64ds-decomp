/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class WallSign: 4 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef WALLSIGN_H
#define WALLSIGN_H
#include "types.h"

struct WallSign {
    u8  pad_000[0xd4];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mMovingCylinderClsnWithPos;            /* 0x320 */
#ifdef __cplusplus
    /* methods */
    int InitResources();
    int Render();
#endif
};

#endif
