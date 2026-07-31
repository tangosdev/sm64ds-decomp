/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Cloud: 5 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CLOUD_H
#define CLOUD_H
#include "types.h"

struct Cloud {
    u8  pad_000[0xd4];
    u8  mModel;            /* 0x0d4 */
#ifdef __cplusplus
    /* methods */
    int InitResources();
    int Render();
#endif
};

#endif
