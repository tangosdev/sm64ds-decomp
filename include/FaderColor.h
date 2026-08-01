/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class FaderColor: 3 matched functions, 2 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FADERCOLOR_H
#define FADERCOLOR_H
#include "types.h"

struct FaderColor {
    u8  pad_000[0x4];
    s32 unk_004;            /* 0x004 */
    u8  pad_008[0x4];
    u16 unk_00c;            /* 0x00c */
#ifdef __cplusplus
    /* methods */
    void AdvanceFade();
#endif
};

#endif
