/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RollingLogLll: 5 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ROLLINGLOGLLL_H
#define ROLLINGLOGLLL_H
#include "types.h"

struct RollingLogLll {
    u8  pad_000[0xd4];
    u8  mMovingCylinderClsn;            /* 0x0d4 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
