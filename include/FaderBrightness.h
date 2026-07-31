/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class FaderBrightness: 8 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FADERBRIGHTNESS_H
#define FADERBRIGHTNESS_H
#include "types.h"

struct FaderBrightness {
    u8  pad_000[0x4];
    s32 unk_004;            /* 0x004 */
#ifdef __cplusplus
    /* methods */
    int IsBetweenStartAndEnd();
    void AdvanceFade();
#endif
};

#endif
