/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PyramidTag: 4 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef PYRAMIDTAG_H
#define PYRAMIDTAG_H
#include "types.h"
#include "dCcAc_c.h"

struct PyramidTag {
    u8  pad_000[0xd4];
    dCcAc_c mdCcAc_c;         /* 0x0d4 */
    s32 mPyramidTopID;            /* 0x108 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

typedef char PyramidTag_size_must_be_0x10c[sizeof(struct PyramidTag) == 0x10c ? 1 : -1];

#endif
