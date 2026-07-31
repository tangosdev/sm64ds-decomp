/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MovingCylinderClsn: 8 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MOVINGCYLINDERCLSN_H
#define MOVINGCYLINDERCLSN_H
#include "types.h"

struct MovingCylinderClsn {
    u8  pad_000[0x30];
    u8  unk_030;            /* 0x030 */
#ifdef __cplusplus
    /* methods */
    int GetOwnerID();
    void * GetPos();
#endif
};

#endif
