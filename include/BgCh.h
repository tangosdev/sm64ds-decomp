/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BgCh: 5 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BGCH_H
#define BGCH_H
#include "types.h"

struct BgCh {
    u8  pad_000[0x4];
    u8  unk_004;            /* 0x004 */
#ifdef __cplusplus
    /* methods */
    void StartDetectingToxic();
    void StartDetectingWater();
    void StopDetectingOrdinary();
    void StopDetectingWater();
#endif
};

#endif
