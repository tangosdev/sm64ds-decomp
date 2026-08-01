/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class CylinderClsnWithPos: 6 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CYLINDERCLSNWITHPOS_H
#define CYLINDERCLSNWITHPOS_H
#include "types.h"

struct CylinderClsnWithPos {
    u8  pad_000[0x30];
    u8  unk_030;            /* 0x030 */
#ifdef __cplusplus
    /* methods */
    struct Vector3* GetPos();
    u32 GetOwnerID();
#endif
};

#endif
