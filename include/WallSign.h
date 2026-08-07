/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class WallSign: 4 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef WALLSIGN_H
#define WALLSIGN_H
#include "types.h"
#include "Model.h"

struct WallSign {
    u8  pad_000[0xd4];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x1fb];
    u8  mMovingCylinderClsnWithPos;            /* 0x320 */
#ifdef __cplusplus
    /* methods */
    int InitResources();
    int Behavior();
    int Render();
#endif
};

#endif
