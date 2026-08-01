/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MegaMushroomCreateTag: 4 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MEGAMUSHROOMCREATETAG_H
#define MEGAMUSHROOMCREATETAG_H
#include "types.h"

struct MegaMushroomCreateTag {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0xc8];
    u8  mMovingCylinderClsn;            /* 0x0d4 */
    u8  pad_0d5[0x34];
    s8  unk_109;            /* 0x109 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
