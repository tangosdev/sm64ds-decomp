/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Wiggler: 4 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef WIGGLER_H
#define WIGGLER_H
#include "types.h"

struct Wiggler {
    u8  pad_000[0x708];
    u8  mWithMeshClsn;            /* 0x708 */
#ifdef __cplusplus
    /* methods */
    int InitResources();
    int Render();
#endif
};

#endif
