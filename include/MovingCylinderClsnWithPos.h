/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MovingCylinderClsnWithPos: 6 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MOVINGCYLINDERCLSNWITHPOS_H
#define MOVINGCYLINDERCLSNWITHPOS_H
#include "types.h"

struct MovingCylinderClsnWithPos {
    u8  pad_000[0x34];
    u8  unk_034;            /* 0x034 */
#ifdef __cplusplus
    /* methods */
    struct Vector3* GetPos();
#endif
};

#endif
