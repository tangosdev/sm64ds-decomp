/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class HeapAllocator: 2 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef HEAPALLOCATOR_H
#define HEAPALLOCATOR_H
#include "types.h"

struct HeapAllocator {
    u8  pad_000[0x18];
    s32 unk_018;            /* 0x018 */
    s32 unk_01c;            /* 0x01c */
    u32 unk_020;            /* 0x020 */
#ifdef __cplusplus
    /* methods */
    void Remove();
#endif
};

#endif
