/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class WaterfallMist: 4 matched functions, 5 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef WATERFALLMIST_H
#define WATERFALLMIST_H
#include "types.h"

struct WaterfallMist {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x6c];
    s16 unk_0d4;            /* 0x0d4 */
    u8  pad_0d6[0x2];
    u8  unk_0d8;            /* 0x0d8 */
#ifdef __cplusplus
    /* methods */
    int InitResources();
#endif
};

#endif
