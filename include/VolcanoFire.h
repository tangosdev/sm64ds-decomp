/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class VolcanoFire: 5 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef VOLCANOFIRE_H
#define VOLCANOFIRE_H
#include "types.h"
#include "dCcAc_c.h"

struct VolcanoFire {
    u8  pad_000[0xd4];
    dCcAc_c mdCcAc_c;         /* 0x0d4 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
