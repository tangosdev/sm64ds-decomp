/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PyramidTag: 4 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef PYRAMIDTAG_H
#define PYRAMIDTAG_H
#include "types.h"

struct PyramidTag {
    u8  pad_000[0xd4];
    u8  mMovingCylinderClsn;            /* 0x0d4 */
    u8  pad_0d5[0x23];
    s32 unk_0f8;            /* 0x0f8 */
    u8  pad_0fc[0xc];
    s32 unk_108;            /* 0x108 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
