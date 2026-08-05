/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class FaderWipe: 5 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FADERWIPE_H
#define FADERWIPE_H
#include "types.h"

/* fwd */
struct id;
struct FaderWipe {
    u8  pad_000[0x4];
    s32 mFadeAmount;            /* 0x004 */
    s32 unk_008;            /* 0x008 */
    u16 unk_00c;            /* 0x00c */
    u8  pad_00e[0x2];
    u8  unk_010;            /* 0x010 */
#ifdef __cplusplus
    /* methods */
    void AdvanceFade();
    void LoadAndSetFile(unsigned short id);
#endif
};

#endif
