/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Trap: 5 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TRAP_H
#define TRAP_H
#include "types.h"

struct Trap {
    u8  pad_000[0xd4];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMovingCylinderClsnWithPos;            /* 0x124 */
    u8  pad_125[0x43];
    u8  unk_168;            /* 0x168 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
